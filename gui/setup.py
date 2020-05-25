from cx_Freeze import setup, Executable

# Dependencies are automatically detected, but it might need
# fine tuning.
buildOptions = dict(packages=["multiprocessing", "pyqtgraph", "pyqtgraph.debug", "pyqtgraph.ThreadsafeTimer"],
                    excludes=[], include_files=["fan.svg", "logo.svg", "gui.css"])

import sys

base = 'Win32GUI' if sys.platform == 'win32' else None

executables = [
    Executable('main.py', base=base, targetName='leviathans_breath.exe')
]

setup(name="Leviathan's Breath",
      version='1.0',
      description="GUI for Leviathan's Breath Fan Controller",
      options=dict(build_exe=buildOptions),
      executables=executables)
