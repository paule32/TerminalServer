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
import os
import subprocess
import json
from datetime import datetime, timedelta

DATA_DIR = "data"
CA_KEY = os.path.join(DATA_DIR, "ca", "ca.key")
CA_CRT = os.path.join(DATA_DIR, "ca", "ca.crt")
CSR_DIR = os.path.join(DATA_DIR, "requests")
CLIENT_DIR = os.path.join(DATA_DIR, "clients")
DB_PATH = os.path.join(DATA_DIR, "clients.json")

os.makedirs(CLIENT_DIR, exist_ok=True)

def load_csrs():
    return [f for f in os.listdir(CSR_DIR) if f.endswith(".csr")]

def sign_csr(csr_filename, days=0, minutes=30):
    basename = os.path.splitext(csr_filename)[0]
    crt_path = os.path.join(CLIENT_DIR, f"{basename}.crt")

    start = datetime.utcnow()
    end = start + timedelta(days=days, minutes=minutes)

    start_str = start.strftime("%Y%m%d%H%M%SZ")
    end_str = end.strftime("%Y%m%d%H%M%SZ")

    cmd = [
        "openssl", "x509", "-req",
        "-in", os.path.join(CSR_DIR, csr_filename),
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

    update_db(basename, start.isoformat(), end.isoformat(), "aktiv")

def update_db(cn, start, end, status):
    db = {}
    if os.path.exists(DB_PATH):
        with open(DB_PATH, "r", encoding="utf-8") as f:
            db = json.load(f)

    db[cn] = {
        "start": start,
        "end": end,
        "status": status
    }

    with open(DB_PATH, "w", encoding="utf-8") as f:
        json.dump(db, f, indent=2)

def load_clients():
    if os.path.exists(DB_PATH):
        with open(DB_PATH, "r", encoding="utf-8") as f:
            return json.load(f)
    return {}

def set_client_status(cn, status):
    clients = load_clients()
    if cn in clients:
        clients[cn]["status"] = status
        with open(DB_PATH, "w", encoding="utf-8") as f:
            json.dump(clients, f, indent=2)
