def getArguments():
    return globals()["_QCalcArguments"]


def setOutputValue(v):
    globals()["_QCalcOutputValue"] = v


print("QCalc module has been loaded")