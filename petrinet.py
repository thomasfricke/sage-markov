#!/usr/bin/env python
"""
Usage:
  petrinet.py <model> <template>
"""

from json import dumps
from yaml import load, dump
from string import Template
from docopt import docopt

try:
    from yaml import CLoader as Loader, CDumper as Dumper
except ImportError:
    from yaml import Loader, Dumper

if __name__ == '__main__':
    arguments = docopt(__doc__, version="v0.1")
    print(arguments)

    model_stream=open(arguments['<model>'])
    model_data = load(model_stream, Loader=Loader)
    output = dump(model_data, Dumper=Dumper)

    template_stream=open(arguments['<template>'])
    template_data = load(template_stream, Loader=Loader)
    output = dump(template_data, Dumper=Dumper)
#    print(output)
#    print(dumps(model_data,indent=4))

    class Model():

        def __init__(self,data,template):
            self.data=data
            self.type=data['type']
            self.template=template


        def header(self):
            header_template=Template(self.template['header'])
            return header_template.substitute(type=self.type.upper())


        def model(self):
            reactants_template=Template(self.template['reactants'])
            reactant_template=Template(self.template['molecule'])

            reactants=""
            for r in self.data['molecule']:
                reactants +=reactant_template.substitute(name=r['name'])

            return reactants_template.substitute(reactants=reactants)


    model=Model(model_data,template_data)
    print model.header()
    print model.model()
