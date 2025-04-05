from PyQt5.QtWidgets import QDialog, QFileDialog
from ui_settings import Ui_SettingsDialog
import configparser
import os

CONFIG_PATH = "settings.ini"

class SettingsDialog(QDialog, Ui_SettingsDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setupUi(self)
        self.load_settings()
        self.btnBrowse.clicked.connect(self.choose_folder)

    def choose_folder(self):
        folder = QFileDialog.getExistingDirectory(self, "Zielverzeichnis wählen")
        if folder:
            self.editExportPath.setText(folder)

    def load_settings(self):
        config = configparser.ConfigParser()
        config.read(CONFIG_PATH)
        if config.has_section("Export"):
            self.editExportPath.setText(config.get("Export", "WebPfad", fallback=""))

    def save_settings(self):
        config = configparser.ConfigParser()
        config["Export"] = {
            "WebPfad": self.editExportPath.text()
        }
        with open(CONFIG_PATH, "w") as f:
            config.write(f)

    def get_export_path(self):
        return self.editExportPath.text()
