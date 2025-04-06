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
import zipfile

DATA_DIR = "data"
CA_KEY = os.path.join(DATA_DIR, "ca", "ca.key")
CA_CRT = os.path.join(DATA_DIR, "ca", "ca.crt")
CSR_DIR = os.path.join(DATA_DIR, "requests")
CLIENT_DIR = os.path.join(DATA_DIR, "clients")
DB_PATH = os.path.join(DATA_DIR, "clients.json")
CRL_PATH = os.path.join(DATA_DIR, "ca", "crl.pem")

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

    update_db(basename, start.isoformat(), end.isoformat(), "aktiv", False)

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

def load_clients():
    if os.path.exists(DB_PATH):
        with open(DB_PATH, "r", encoding="utf-8") as f:
            return json.load(f)
    return {}

def set_client_status(cn, status):
    clients = load_clients()
    if cn in clients:
        clients[cn]["status"] = status
        clients[cn]["revoked"] = (status == "gesperrt")
        with open(DB_PATH, "w", encoding="utf-8") as f:
            json.dump(clients, f, indent=2)
        generate_crl(clients)

def generate_crl(clients):
    index_file = os.path.join(DATA_DIR, "ca", "index.txt")
    with open(index_file, "w", encoding="utf-8") as idx:
        for cn, info in clients.items():
            if info.get("revoked"):
                expiry = datetime.strptime(info["end"], "%Y-%m-%dT%H:%M:%S.%f")
                serial = "R{:X}".format(abs(hash(cn)) % (10 ** 8))
                line = f"R	{expiry.strftime('%y%m%d%H%M%SZ')}		{serial}	unknown
"
                idx.write(line)

    cmd = [
        "openssl", "ca", "-gencrl",
        "-keyfile", CA_KEY,
        "-cert", CA_CRT,
        "-out", CRL_PATH,
        "-config", "openssl.cnf"
    ]
    subprocess.run(cmd)

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
