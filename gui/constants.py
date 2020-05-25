import os
import sys

try:
    APP_ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
except NameError:
    APP_ROOT_DIR = os.path.dirname(sys.executable)
if getattr(sys, "frozen", False):
    APP_ROOT_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

MAX_TIME_SERIES_POINTS = 60
DB_FILE_NAME = os.path.join(APP_ROOT_DIR, "db.yaml")
