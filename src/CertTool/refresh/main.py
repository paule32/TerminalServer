import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QMessageBox, QTableWidgetItem
from PyQt5.QtCore import QTimer
from ui_mainwindow import Ui_MainWindow
import ca_manager

class CertManager(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)

        self.btnLoadCSRs.clicked.connect(self.load_csrs)
        self.btnSignSelected.clicked.connect(self.sign_selected)

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.refresh_clients)
        self.timer.start(10000)  # alle 10 Sekunden

        self.refresh_clients()

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
            row_color = "background-color: #FFCCCC;" if cn in expired else ""
            for col in range(3):
                self.tableClients.item(row, col).setBackground(
                    QtGui.QColor("#FFCCCC") if cn in expired else QtGui.QColor("#FFFFFF"))

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = CertManager()
    window.show()
    sys.exit(app.exec_())
