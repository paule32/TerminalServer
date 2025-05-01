# ---------------------------------------------------------------------------------------
# Copyright(c) 2025 Jens Kallup
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this
# software and associated documentation files(the "Software"), to deal in the Software
# without restriction, including without limitation the rights to use, copy, modify,
# merge, publish, distribute, sublicense, and /or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to the following
# conditions :
#
# The above copyright notice and this permission notice shall be included in all copies
# or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
# CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# ---------------------------------------------------------------------------------------
import sys

import urllib.request

import os
import subprocess
import json
from datetime import datetime, timedelta
import zipfile

# ------------------------------------------------------------------------
# Qt5 gui framework
# ------------------------------------------------------------------------
from PyQt5.QtWidgets            import *
from PyQt5.QtCore               import *
from PyQt5.QtGui                import *
from PyQt5.QtNetwork            import *

VERSION = "1.0.0"
UPDATE_URL = "http://domain.local/version.txt"  # Lokales Netz

DATA_DIR = "data"

CA_KEY = os.path.join(DATA_DIR, "CA", ".")
CA_KEY = os.path.join(CA_KEY, "private", "ca.key")
#
CA_CRT = os.path.join(DATA_DIR, "CA", ".")
CA_CRT = os.path.join(CA_CRT, "public", "ca.crt")

print("---> " + CA_KEY)
print("---> " + CA_CRT)

CSR_DIR    = os.path.join(DATA_DIR, "requests")
CLIENT_DIR = os.path.join(DATA_DIR, "clients")
DB_PATH    = os.path.join(DATA_DIR, "clients.json")
CRL_PATH   = os.path.join(DATA_DIR, "ca", "crl.pem")

os.makedirs(CLIENT_DIR, exist_ok=True)

def find_csr_files(root_dir):
    csr_files = []
    for dirpath, dirnames, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.lower().endswith('.csr'):
                full_path = os.path.join(dirpath, filename)
                csr_files.append(full_path)
    return csr_files

def load_csrs():
    return find_csr_files("data/CA")

def update_db(cn, start, end, status, revoked):
    db = {}
    if os.path.exists(DB_PATH):
        with open(DB_PATH, "r", encoding="utf-8") as f:
            db = json.load(f)
    
    db[cn] = {
        "start": start,
        "end": end,
        "status": status,
        "revoked": revoked
    }
    
    with open(DB_PATH, "w", encoding="utf-8") as f:
        json.dump(db, f, indent=2)

def sign_csr(csr_filename, days=0, minutes=30):
    basename = os.path.splitext(csr_filename)[0]
    crt_path = os.path.join(CLIENT_DIR, f"{basename}.crt")
    
    start = datetime.utcnow()
    end = start + timedelta(days=days, minutes=minutes)
    
    start_str = start.strftime("%Y%m%d%H%M%SZ")
    end_str = end.strftime("%Y%m%d%H%M%SZ")
    
    cmd = [
        "openssl", "x509", "-req",
        "-in", csr_filename,
        "-CA", CA_CRT, "-CAkey", CA_KEY,
        "-CAcreateserial",
        "-out", crt_path,
        "-startdate", start_str,
        "-enddate", end_str,
        "-sha256"
    ]
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError("Signierung fehlgeschlagen: " + result.stderr)
    
    update_db(basename, start.isoformat(), end.isoformat(), "aktiv", False)

def load_clients():
    if os.path.exists(DB_PATH):
        with open(DB_PATH, "r", encoding="utf-8") as f:
            return json.load(f)
    return {}

def generate_crl(clients):
    index_file = os.path.join(DATA_DIR, "ca", "index.txt")
    with open(index_file, "w", encoding="utf-8") as idx:
        for cn, info in clients.items():
            if info.get("revoked"):
                expiry = datetime.strptime(info["end"], "%Y-%m-%dT%H:%M:%S.%f")
                serial = "R{:X}".format(abs(hash(cn)) % (10 ** 8))
                line = ("	" + expiry.strftime('%y%m%d%H%M%SZ') + "		" + serial + "	unknown")
                idx.write(line)
    
    cmd = [
        "openssl", "ca", "-gencrl",
        "-keyfile", CA_KEY,
        "-cert", CA_CRT,
        "-out", CRL_PATH,
        "-config", "openssl.cnf"
    ]
    subprocess.run(cmd)

def set_client_status(cn, status):
    clients = load_clients()
    if cn in clients:
        clients[cn]["status"] = status
        clients[cn]["revoked"] = (status == "gesperrt")
        with open(DB_PATH, "w", encoding="utf-8") as f:
            json.dump(clients, f, indent=2)
        generate_crl(clients)

def export_client_bundle(cn):
    crt = os.path.join(CLIENT_DIR, f"{cn}.crt")
    key = os.path.join(CLIENT_DIR, f"{cn}.key")
    bundle = os.path.join(CLIENT_DIR, f"{cn}_bundle.zip")
    with zipfile.ZipFile(bundle, "w") as z:
        z.write(crt, f"{cn}.crt")
        if os.path.exists(key):
            z.write(key, f"{cn}.key")
        z.write(CA_CRT, "ca.crt")
    return bundle

def check_expired_clients():
    now = datetime.utcnow()
    expired = []
    clients = load_clients()
    for cn, info in clients.items():
        end = datetime.strptime(info["end"], "%Y-%m-%dT%H:%M:%S.%f")
        if end < now:
            expired.append(cn)
    return expired

def refresh_tabs():  # Platzhalter für spätere GUI-Anbindung
    pass

class CertManager(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setObjectName("MainWindow")
        self.resize(800,550)
        self.centralwidget = QWidget(self)
        self.centralwidget.setObjectName("centralwidget")
        self.tabWidget = QTabWidget(self.centralwidget)
        self.tabWidget.setGeometry(QRect(10, 10, 780, 500))
        self.tabWidget.setObjectName("tabWidget")
        self.tab_server = QWidget()
        self.tab_server.setObjectName("tab_server")
        self.btnLoadCSRs = QPushButton(self.tab_server)
        self.btnLoadCSRs.setGeometry(QRect(10, 10, 180, 30))
        self.btnLoadCSRs.setObjectName("btnLoadCSRs")
        self.btnSignSelected = QPushButton(self.tab_server)
        self.btnSignSelected.setGeometry(QRect(200, 10, 180, 30))
        self.btnSignSelected.setObjectName("btnSignSelected")
        self.tableCSRs = QTableWidget(self.tab_server)
        self.tableCSRs.setGeometry(QRect(10, 50, 740, 400))
        self.tableCSRs.setObjectName("tableCSRs")
        self.tableCSRs.setColumnCount(0)
        self.tableCSRs.setRowCount(0)
        self.btnExportCert = QPushButton(self.tableCSRs)
        self.btnExportCert.setGeometry(QRect(10, 460, 160, 30))
        self.btnExportCert.setObjectName("btnExportCert")
        self.btnRevoke = QPushButton(self.tableCSRs)
        self.btnRevoke.setGeometry(QRect(180, 460, 100, 30))
        self.btnRevoke.setObjectName("btnRevoke")
        self.btnExportAll = QPushButton(self.tableCSRs)
        self.btnExportAll.setGeometry(QRect(400, 460, 150, 30))
        self.btnExportAll.setObjectName("btnExportAll")
        self.btnUnrevoke = QPushButton(self.tableCSRs)
        self.btnUnrevoke.setGeometry(QRect(290, 460, 100, 30))
        self.btnUnrevoke.setObjectName("btnUnrevoke")
        self.tabWidget.addTab(self.tab_server, "")
        self.tab_clients = QWidget()
        self.tab_clients.setObjectName("tab_clients")
        self.searchInput = QLineEdit(self.tab_clients)
        self.searchInput.setGeometry(QRect(10, 10, 400, 25))
        self.searchInput.setObjectName("searchInput")
        self.tableClients = QTableWidget(self.tab_clients)
        self.tableClients.setGeometry(QRect(10, 10, 740, 440))
        self.tableClients.setObjectName("tableClients")
        self.tableClients.setColumnCount(0)
        self.tableClients.setRowCount(0)
        self.btnExportCert1 = QPushButton(self.tableClients)
        self.btnExportCert1.setGeometry(QRect(10, 460, 160, 30))
        self.btnExportCert1.setObjectName("btnExportCert1")
        self.btnRevoke1 = QPushButton(self.tableClients)
        self.btnRevoke1.setGeometry(QRect(180, 460, 100, 30))
        self.btnRevoke1.setObjectName("btnRevoke1")
        self.btnExportAll1 = QPushButton(self.tableClients)
        self.btnExportAll1.setGeometry(QRect(400, 460, 150, 30))
        self.btnExportAll1.setObjectName("btnExportAll1")
        self.btnUnrevoke1 = QPushButton(self.tableClients)
        self.btnUnrevoke1.setGeometry(QRect(290, 460, 100, 30))
        self.btnUnrevoke1.setObjectName("btnUnrevoke1")
        self.tabWidget.addTab(self.tab_clients, "")
        self.setCentralWidget(self.centralwidget)
        
        self.retranslateUi(self)
        self.setWindowTitle("SSL-Zertifikatsverwaltung - (c) 2025 Jens Kallup - paule32")
        
        self.btnLoadCSRs.clicked.connect(self.load_csrs)
        self.btnSignSelected.clicked.connect(self.sign_selected)
        self.btnExportCert.clicked.connect(self.export_selected_cert)
        self.btnRevoke.clicked.connect(self.revoke_selected)
        self.btnExportAll.clicked.connect(self.export_all_data)
        self.btnUnrevoke.clicked.connect(self.unrevoke_selected)
        
        self.tableClients.setContextMenuPolicy(Qt.CustomContextMenu)
        self.tableClients.customContextMenuRequested.connect(self.show_context_menu)
        
        self.searchInput.textChanged.connect(self.refresh_clients)
        
        self.init_menu()
        
        # Timer für Refresh & Updateprüfung
        self.refresh_timer = QTimer(self)
        self.refresh_timer.timeout.connect(self.refresh_clients)
        self.refresh_timer.start(10000)
        
        self.update_timer = QTimer(self)
        self.update_timer.timeout.connect(self.check_for_update)
        self.update_timer.start(6 * 60 * 60 * 1000)  # alle 6 Stunden
        
        self.refresh_clients()
    
    def retranslateUi(self, MainWindow):
        _translate = QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "SSL-Zertifikatsverwaltung"))
        self.btnLoadCSRs.setText(_translate("MainWindow", "CSR-Dateien laden"))
        self.btnSignSelected.setText(_translate("MainWindow", "Zertifikat signieren"))
        self.btnExportCert.setText(_translate("MainWindow", "Zertifikat exportieren"))
        self.btnRevoke.setText(_translate("MainWindow", "Sperren"))
        self.btnExportAll.setText(_translate("MainWindow", "Daten exportieren"))
        self.btnUnrevoke.setText(_translate("MainWindow", "Freigeben"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_server), _translate("MainWindow", "CA / Server"))
        self.searchInput.setPlaceholderText(_translate("MainWindow", "Client-Name suchen..."))
        self.btnExportCert1.setText(_translate("MainWindow", "Zertifikat exportieren"))
        self.btnRevoke1.setText(_translate("MainWindow", "Sperren"))
        self.btnExportAll1.setText(_translate("MainWindow", "Daten exportieren"))
        self.btnUnrevoke1.setText(_translate("MainWindow", "Freigeben"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_clients), _translate("MainWindow", "Clients"))
    
    def init_menu(self):
        menubar = self.menuBar()
        file_menu = menubar.addMenu("&Datei")
        help_menu = menubar.addMenu("&Hilfe")
        
        export_action = QAction("Daten exportieren...", self)
        export_action.triggered.connect(self.export_all_data)
        file_menu.addAction(export_action)
        
        exit_action = QAction("Beenden", self)
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)
        
        about_action = QAction("Über dieses Tool", self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)
    
    def show_about(self):
        QMessageBox.information(self, "Über",(
        "Zertifikatsverwaltung\n        Version: " + VERSION + "\n  (c) 2025 by Jens Kallup"))
    
    def check_for_update(self):
        try:
            with urllib.request.urlopen(UPDATE_URL, timeout=5) as response:
                latest = response.read().decode().strip()
                if latest != VERSION:
                    QMessageBox.information(self, "Update verfügbar",(
                    "Neue Version verfügbar: " + latest + 
                    "Aktuelle Version: " + VERSION ))
        except Exception as e:
            print("Updateprüfung fehlgeschlagen:", e)
    
    def show_context_menu(self, pos):
        menu = QMenu(self)
        menu.addAction("Zertifikat exportieren", self.export_selected_cert)
        menu.addAction("Sperren", self.revoke_selected)
        menu.addAction("Freigeben", self.unrevoke_selected)
        menu.exec_(self.tableClients.mapToGlobal(pos))
    
    def load_csrs(self):
        self.tableCSRs.setRowCount(0)
        self.tableCSRs.setColumnCount(1)
        self.tableCSRs.setColumnWidth(0, 420)
        csrs = load_csrs()
        for row, filename in enumerate(csrs):
            self.tableCSRs.insertRow(row)
            self.tableCSRs.setItem(row, 0, QTableWidgetItem(filename))
    
    def sign_selected(self):
        selected = self.tableCSRs.currentRow()
        if selected == -1:
            QMessageBox.warning(self, "Fehler", "Bitte eine CSR-Datei auswählen.")
            return
        filename = self.tableCSRs.item(selected, 0).text()
        try:
            sign_csr(filename)
            QMessageBox.information(self, "Erfolg", f"{filename} wurde signiert.")
            self.refresh_clients()
        except Exception as e:
            QMessageBox.critical(self, "Fehler", str(e))
    
    def refresh_clients(self):
        filter_text = self.searchInput.text().lower()
        self.tableClients.setRowCount(0)
        clients = load_clients()
        expired = check_expired_clients()
        for row, (cn, info) in enumerate(clients.items()):
            if filter_text and filter_text not in cn.lower():
                continue
            self.tableClients.insertRow(self.tableClients.rowCount())
            r = self.tableClients.rowCount() - 1
            self.tableClients.setItem(r, 0, QTableWidgetItem(cn))
            self.tableClients.setItem(r, 1, QTableWidgetItem(info["end"]))
            self.tableClients.setItem(r, 2, QTableWidgetItem(info["status"]))
            tooltip = f"Ablauf: {info['end']}\nStatus: {info['status']}"
            if info["revoked"]:
                tooltip += "\nZertifikat ist gesperrt."
            if cn in expired:
                tooltip += "\nZertifikat ist abgelaufen."
            for col in range(3):
                self.tableClients.item(r, col).setToolTip(tooltip)
                if cn in expired:
                    self.tableClients.item(r, col).setBackground(QtGui.QColor("#FFCCCC"))
    
    def export_selected_cert(self):
        selected = self.tableClients.currentRow()
        if selected == -1:
            QMessageBox.warning(self, "Fehler", "Bitte einen Client auswählen.")
            return
        cn = self.tableClients.item(selected, 0).text()
        path = export_client_bundle(cn)
        QMessageBox.information(self, "Exportiert", ("Exportiert nach: " + path))
    
    def revoke_selected(self):
        selected = self.tableClients.currentRow()
        if selected == -1:
            QMessageBox.warning(self, "Fehler", "Bitte einen Client auswählen.")
            return
        cn = self.tableClients.item(selected, 0).text()
        set_client_status(cn, "gesperrt")
        self.refresh_clients()
    
    def unrevoke_selected(self):
        selected = self.tableClients.currentRow()
        if selected == -1:
            QMessageBox.warning(self, "Fehler", "Bitte einen Client auswählen.")
            return
        cn = self.tableClients.item(selected, 0).text()
        set_client_status(cn, "aktiv")
        self.refresh_clients()
    
    def export_all_data(self):
        from zipfile import ZipFile
        import os
        
        timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M")
        export_dir = os.path.join("data", "exports")
        os.makedirs(export_dir, exist_ok=True)
        export_path = os.path.join(export_dir, f"certmanager_export_{timestamp}.zip")
        
        with ZipFile(export_path, "w") as z:
            # CRL-Datei
            crl_path = os.path.join("data", "ca", "crl.pem")
            if os.path.exists(crl_path):
                z.write(crl_path, arcname="crl.pem")
            # JSON-Datenbank
            db_path = os.path.join("data", "clients.json")
            if os.path.exists(db_path):
                z.write(db_path, arcname="clients.json")
            # Alle Bundles
            bundle_dir = os.path.join("data", "clients")
            for fname in os.listdir(bundle_dir):
                if fname.endswith("_bundle.zip"):
                    z.write(os.path.join(bundle_dir, fname), arcname=fname)
        
        QMessageBox.information(self, "Export abgeschlossen", (
        "Exportiert nach: " + export_path))

if __name__ == "__main__":
    app = QApplication(sys.argv)
    ca_manager = CertManager();
    ca_manager.show()
    sys.exit(app.exec_())
