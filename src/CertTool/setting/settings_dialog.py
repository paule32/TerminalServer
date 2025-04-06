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
