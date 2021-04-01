import qci


def register(symbolname, callback, enableargs):
    qci.registerScriptFunction(symbolname, callback, enableargs)


def unregister(symbolname):
    qci.unregisterScriptFunction(symbolname)