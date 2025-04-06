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
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QMessageBox, QTableWidgetItem,
    QMenu, QAction, QLineEdit
)
from PyQt5.QtCore import QTimer, Qt
from PyQt5 import QtGui
from ui_mainwindow import Ui_MainWindow
import ca_manager
import urllib.request

VERSION = "1.0.0"
UPDATE_URL = "http://192.168.1.100/certmanager/version.txt"  # Lokales Netz

class CertManager(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.setWindowTitle("SSL-Zertifikatsverwaltung")

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

    def init_menu(self):
        menubar = self.menuBar()
        file_menu = menubar.addMenu("&Datei")
        help_menu = menubar.addMenu("&Hilfe")

        exit_action = QAction("Beenden", self)
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)

        about_action = QAction("Über dieses Tool", self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)

    def show_about(self):
        QMessageBox.information(self, "Über",
            f"Zertifikatsverwaltung
Version {VERSION}
(c) 2025 TLS-Toolchain")

            export_action = QAction("Daten exportieren...", self)
        export_action.triggered.connect(self.export_all_data)
        file_menu.addAction(export_action)

    def check_for_update(self):
        try:
            with urllib.request.urlopen(UPDATE_URL, timeout=5) as response:
                latest = response.read().decode().strip()
                if latest != VERSION:
                    QMessageBox.information(self, "Update verfügbar",
                        f"Neue Version verfügbar: {latest}
Aktuelle Version: {VERSION}")
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
        csrs = ca_manager.load_csrs()
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
            ca_manager.sign_csr(filename)
            QMessageBox.information(self, "Erfolg", f"{filename} wurde signiert.")
            self.refresh_clients()
        except Exception as e:
            QMessageBox.critical(self, "Fehler", str(e))

    def refresh_clients(self):
        filter_text = self.searchInput.text().lower()
        self.tableClients.setRowCount(0)
        clients = ca_manager.load_clients()
        expired = ca_manager.check_expired_clients()
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
        path = ca_manager.export_client_bundle(cn)
        QMessageBox.information(self, "Exportiert", f"Exportiert nach:
{path}")

    def revoke_selected(self):
        selected = self.tableClients.currentRow()
        if selected == -1:
            QMessageBox.warning(self, "Fehler", "Bitte einen Client auswählen.")
            return
        cn = self.tableClients.item(selected, 0).text()
        ca_manager.set_client_status(cn, "gesperrt")
        self.refresh_clients()

    def unrevoke_selected(self):
        selected = self.tableClients.currentRow()
        if selected == -1:
            QMessageBox.warning(self, "Fehler", "Bitte einen Client auswählen.")
            return
        cn = self.tableClients.item(selected, 0).text()
        ca_manager.set_client_status(cn, "aktiv")
        self.refresh_clients()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = CertManager()
    window.show()
    sys.exit(app.exec_())

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

        QMessageBox.information(self, "Export abgeschlossen",
            f"Exportiert nach:
{export_path}")
