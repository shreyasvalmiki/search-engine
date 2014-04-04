import re
from queue import Queue

reFnameMatch = re.compile('.*/[0-9]+_[a-zA-z]+$')
reFnameSplit = re.compile('_[a-zA-z]+$')
reWordSplit = re.compile('[\W\s]+')
reFirstChar = re.compile('[a-z0-9]')
q = Queue()
wordSets = set()
urlId = 0
