#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
=========================================================
Linear Regression Example
=========================================================
This example uses the only the first feature of the `diabetes` dataset, in
order to illustrate a two-dimensional plot of this regression technique. The
straight line can be seen in the plot, showing how linear regression attempts
to draw a straight line that will best minimize the residual sum of squares
between the observed responses in the dataset, and the responses predicted by
the linear approximation.

The coefficients, the residual sum of squares and the variance score are also
calculated.

"""
print(__doc__)


# Code source: Jaques Grobler
# License: BSD 3 clause


import matplotlib.pyplot as plt
import numpy as np
from sklearn import datasets, linear_model
from sklearn.metrics import mean_squared_error, r2_score
import pandas
import itertools
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--data', type=str, help="data csv file.", default="test.csv")
parser.add_argument('--sparsity', type=int, help="data csv file.", default=2)

args = parser.parse_args()

df = pandas.read_csv(args.data, header=0)
df = df.values
featureNumber = df[0].size - 1

features = df[:, slice(0,featureNumber)]
target = df[:, featureNumber]

s = args.sparsity

print("Featuers:")
print(features)
print("Target:")
print(target)


def linear_regression(features, target):
	# Create linear regression object
	regr = linear_model.LinearRegression()

	# Train the model using the training sets
	regr.fit(features, target)

	# Make predictions using the testing set
	target_pred = regr.predict(features)

	# The coefficients
	print('Coefficients: \n', regr.coef_)
	# The mean squared error
	print("Mean squared error: %.2f" % mean_squared_error(target, target_pred))
	# Explained variance score: 1 is perfect prediction
	print('Variance score: %.2f' % r2_score(target, target_pred))


def sparse_linear_regression(full_features, target, s):
	best_sigma = 0
	best_error = -1
	best_coefficients = 0

	for sigma in list(itertools.combinations(range(0, featureNumber), s)):
#		print(np.array(sigma))
#		print("features transpose")
		features = (full_features.transpose()[np.array(sigma)]).transpose()

		print("Testing features {}".format(sigma))

		regr = linear_model.LinearRegression()
		regr.fit(features, target)
		target_pred = regr.predict(features)
		coefficients = regr.coef_
		myPred = features.dot(coefficients)
		print("    Target = {}".format(target))
		print("    coefficient = {}".format(coefficients))
		print("    Prediction = {}".format(target_pred))
		print("    my pred = {}".format(myPred))
		normsqr = lambda x: np.inner(x, x)
		error = normsqr(target - target_pred)
		print("    error = {}".format(error))
		print("")

		if best_error == -1:
			best_error = error
			best_coefficients = coefficients
			best_sigma = sigma
		elif best_error > error:
			best_error = error
			best_coefficients = coefficients
			best_sigma = sigma

	print("Best is {}".format(best_sigma))
	print("    coefficients = {}".format(best_coefficients))
	print("    error = {}".format(best_error))
	return (best_error, best_sigma, best_coefficients)


(error, sigma, coef) = sparse_linear_regression(features, target, s)
