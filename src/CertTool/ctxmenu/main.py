import sys
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QMessageBox, QTableWidgetItem,
    QMenu, QAction
)
from PyQt5.QtCore import QTimer, Qt
from PyQt5 import QtGui
from ui_mainwindow import Ui_MainWindow
import ca_manager

class CertManager(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.setWindowTitle("SSL-Zertifikatsverwaltung")

        self.btnLoadCSRs.clicked.connect(self.load_csrs)
        self.btnSignSelected.clicked.connect(self.sign_selected)
        self.btnExportCert.clicked.connect(self.export_selected_cert)
        self.btnRevoke.clicked.connect(self.revoke_selected)
        self.btnUnrevoke.clicked.connect(self.unrevoke_selected)

        self.tableClients.setContextMenuPolicy(Qt.CustomContextMenu)
        self.tableClients.customContextMenuRequested.connect(self.show_context_menu)

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.refresh_clients)
        self.timer.start(10000)

        self.init_menu()
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
            "Zertifikatsverwaltung
Version 1.0
(c) 2025 TLS-Toolchain")

    def show_context_menu(self, pos):
        menu = QMenu(self)
        export_action = QAction("Zertifikat exportieren", self)
        revoke_action = QAction("Sperren", self)
        unrevoke_action = QAction("Freigeben", self)

        export_action.triggered.connect(self.export_selected_cert)
        revoke_action.triggered.connect(self.revoke_selected)
        unrevoke_action.triggered.connect(self.unrevoke_selected)

        menu.addAction(export_action)
        menu.addAction(revoke_action)
        menu.addAction(unrevoke_action)

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
        self.tableClients.setRowCount(0)
        clients = ca_manager.load_clients()
        expired = ca_manager.check_expired_clients()
        for row, (cn, info) in enumerate(clients.items()):
            self.tableClients.insertRow(row)
            self.tableClients.setItem(row, 0, QTableWidgetItem(cn))
            self.tableClients.setItem(row, 1, QTableWidgetItem(info["end"]))
            self.tableClients.setItem(row, 2, QTableWidgetItem(info["status"]))

            tooltip = f"Ablauf: {info['end']}\nStatus: {info['status']}"
            if info["revoked"]:
                tooltip += "\nZertifikat ist gesperrt."
            if cn in expired:
                tooltip += "\nZertifikat ist abgelaufen."

            for col in range(3):
                self.tableClients.item(row, col).setToolTip(tooltip)
                if cn in expired:
                    self.tableClients.item(row, col).setBackground(QtGui.QColor("#FFCCCC"))

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
