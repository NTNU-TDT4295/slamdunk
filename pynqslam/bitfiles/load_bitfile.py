from pynq import Overlay
from os import path

current_dir = path.abspath(path.dirname(__file__))
bitfile = path.join(current_dir, "stable.bit")

o = Overlay(bitfile)
o.download()
