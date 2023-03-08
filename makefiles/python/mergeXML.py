"""
Script to merge two (or more) similar XML files.

http://stackoverflow.com/questions/14878706/merge-xml-files-with-nested-elements-without-external-libraries

Authors: jadkik94, Joseph Thomas
"""

from xml.etree import ElementTree as et

class hashabledict(dict):
    def __hash__(self):
        return hash(tuple(sorted(self.items())))

class XMLCombiner(object):
    def __init__(self, filenames):
        assert len(filenames) > 0, 'No filenames!'
        # save all the roots, in order, to be processed later
        self.roots = []
        for f in filenames:
            print("Reading %s" % f)
            self.roots.append(et.parse(f).getroot())

    def combine(self):
        for r in self.roots[1:]:
            # combine each element with the first one, and update that
            self.combine_element(self.roots[0], r)
        # return the string representation
        return et.ElementTree(self.roots[0])

    def combine_element(self, one, other):
        """
        This function recursively updates either the text or the children
        of an element if another element is found in `one`, or adds it
        from `other` if not found.
        """
        # Create a mapping from tag name to element, as that's what we are fltering with
        mapping = {}
        for el in one: mapping[(el.tag, hashabledict(el.attrib))] = el
        for el in other:
            if len(el) == 0:
                # Not nested
                try:
                    # Update the text
                    mapping[(el.tag, hashabledict(el.attrib))].text = el.text
                except KeyError:
                    # An element with this name is not in the mapping
                    mapping[(el.tag, hashabledict(el.attrib))] = el
                    # Add it
                    one.append(el)
            else:
                try:
                    # Recursively process the element, and update it in the same way
                    self.combine_element(mapping[(el.tag, hashabledict(el.attrib))], el)
                except KeyError:
                    # Not in the mapping
                    mapping[(el.tag, hashabledict(el.attrib))] = el
                    # Just add it
                    one.append(el)

if __name__ == '__main__':

    import sys
    import getopt

    opts,args = getopt.getopt(sys.argv[1:], "v")

    verbose = False
    for opt in opts:
        if opt[0] == "-v": verbose = True

    if len(args) < 3:
        raise Exception("Expecting input files (>1) plus output file on command line")

    inputs = args[:-1]
    output = args[-1]

    result = XMLCombiner(inputs).combine()

    if verbose:
        print('-'*20)
        print(et.tostring(result.getroot()))

    print("Writing %s" % output)
    ofp = open(output, "w")
    ofp.write('<?xml version="1.0"?>\n')
    result.write(ofp)
    ofp.write("\n")




