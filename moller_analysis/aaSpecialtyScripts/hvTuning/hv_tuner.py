import csv
import fileinput
import numpy as np
import matplotlib.pyplot as plt
import itertools
import sys
from scipy.stats import pearsonr
from matplotlib import rc

import pandas as pd
from sklearn.utils import shuffle
import timeit
#from wand.image import Image as WImage
from scipy.optimize import curve_fit

from sklearn import linear_model
from sklearn import preprocessing
from sklearn import metrics, svm, preprocessing

from sklearn import model_selection
from sklearn.model_selection import ShuffleSplit
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from scipy.interpolate import UnivariateSpline
from scipy.interpolate import BSpline
from itertools import chain

from sklearn.model_selection import GridSearchCV
from sklearn.model_selection import RandomizedSearchCV
from sklearn.model_selection import KFold
from sklearn.model_selection import learning_curve
from sklearn.model_selection import cross_val_score
from sklearn.model_selection import cross_val_predict
from sklearn.model_selection import KFold, StratifiedKFold
from sklearn.svm import SVC
from sklearn.svm import SVR
from sklearn.ensemble import RandomForestClassifier
from sklearn.ensemble import ExtraTreesClassifier
from sklearn.metrics import confusion_matrix
from sklearn import tree
from sklearn.tree import export_graphviz
from sklearn.metrics import classification_report
from sklearn import *

from sklearn.ensemble import RandomForestRegressor
from scipy import stats


from math import pi
import unicodedata as ud

import unicodedata as ud
from mpl_toolkits.mplot3d import axes3d
from matplotlib import cm



##########################################################
df_pmt = pd.read_csv("firstinput.csv", header=None)
df_now = pd.read_csv("mlinput.csv", header=None)
##########################################################

y = df_pmt[0].values
X = df_pmt.drop(0, axis=1).values


X_now = df_now.values

regressor = RandomForestRegressor(n_jobs = 18)

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.1, random_state=42)
regressor.fit(X_train, y_train)

y_pred = regressor.predict(X_now)

for i in range(8):
    print(r'Change the HV value channel '+str(i+1) +' to ' + str(y_pred[i]))

    

