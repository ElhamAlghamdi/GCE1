import random, os, itertools
import time
from optparse import OptionParser
from datetime import datetime

must_constraints = set()
pair = (2,1)
pair1 = (1,5)
pair2 = (1,4)
must_constraints.add( pair)
must_constraints.add( pair1)
must_constraints.add( pair2)

#sets = [{1,2}, {1,5}, {3,4}, {5,4}]

import re
str = "(387, 625) M";

str=str.replace('(',' ')
str=str.replace(', ',' ')
str=str.replace(') ',' ')
print (str)
