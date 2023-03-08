"""
Indexing functions to enable us to make forward references within the
generated user guide.
"""

class ServiceIndex:
    def __init__(self):
        self.indexBaseDerived = {}
        self.indexClassConstructor = {}
        self.numClassMethods = 0

    def AddDerivedClass(self, baseClass, derivedClass):
        if baseClass not in self.indexBaseDerived:
            self.indexBaseDerived[baseClass] = set()
        self.indexBaseDerived[baseClass].add(derivedClass)

    def AddConstructor(self, cls, constructor):
        if cls not in self.indexClassConstructor:
            self.indexClassConstructor[cls] = set()
        self.indexClassConstructor[cls].add(constructor)

    def IncrementNumClassMethods(self):
        self.numClassMethods += 1

    def DerivedClasses(self, baseClass):
        if baseClass not in self.indexBaseDerived:
            self.indexBaseDerived[baseClass] = set()
        return self.indexBaseDerived[baseClass]

    def Constructors(self, cls):
        if cls not in self.indexClassConstructor:
            self.indexClassConstructor[cls] = set()
        return self.indexClassConstructor[cls]

    def NumClassMethods(self):
        return self.numClassMethods

