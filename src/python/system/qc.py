# The higher level python wrapper around the native interface.
# User addons should use this wrapper as it will be the stable counter part
# to the native interface. This way the native interface can be changed without affecting user addons by changing the
# interface implementation.
# However python does not provide ways of isolating modules so all python code
# can import the native interface directly.
