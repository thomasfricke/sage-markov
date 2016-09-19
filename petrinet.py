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



#    print(output)
#    print(dumps(model_data,indent=4))

class Tree(object):
    def __init__(self, d):
        for a, b in d.items():
            if isinstance(b, (list, tuple)):
                setattr(self, a, [Tree(x) if isinstance(x, dict) else x for x in b])
            else:
                setattr(self, a, Tree(b) if isinstance(b, dict) else b)


class Model(object):

    def __init__(self, template, data):
        self.data = Tree(data)
        print dump(self.data, Dumper=Dumper)
        self.type = self.data.type
        self.template = Tree(template)

    def header(self):
        header_template = Template(self.template.header)
        return header_template.substitute(type=self.type.upper())

    def model(self):
        reactants_template = Template(self.template.reactants)
        reactant_template = Template(self.template.molecule)

        reactants = ""
        for r in self.data.molecules:
            m = r.molecule
            reactants += reactant_template.substitute(name=m.name)

        return reactants_template.substitute(reactants=reactants)

    def educt_factor(self, educts):
        mult = ""
        result = ""
        for e in educts:
            result += mult + self.template.source + e
            mult = " * "

        return result

    def reaction_reactivity(self):
        reactivity_body_template = Template(self.template.reaction.reactivity.body)
        reactivity_name_template = Template(self.template.reaction.reactivity.name)

        rate_template = Template(self.template.reaction.rate)

        result = ""
        for r in self.data.reactions:
            reaction = r.reaction
            name = reactivity_name_template.substitute(name=reaction.name)
            rate = rate_template.substitute(name=name)
            result += reactivity_body_template.substitute(
                name=reaction.name,
                educts=self.educt_factor(reaction.educts),
                rate=rate)

        return result

    def diffusion_rate(self):
        topology_template = Template(self.template.diffusion.topology)
        reactivity_name_template = Template(self.template.diffusion.reactivity.name)
        reactivity_body_template = Template(self.template.diffusion.reactivity.body)

        diffusion  = self.data.diffusion

        result = topology_template.substitute(topology=diffusion.topology.name)

        for m in diffusion.molecules:
            name = reactivity_name_template.substitute(molecule=m.molecule.name)
            result += reactivity_body_template.substitute(name=name,molecule=m.molecule.name)

        return result

    def diffusion_step(self):
        step_template = Template(self.template.diffusion.step)

        result = ""
        for m in self.data.diffusion.molecules:
            result += step_template.substitute(molecule=m.molecule.name)

        return result

    def declaration(self):
        declaration_template = Template(self.template.declaration)
        rate_template = Template(self.template.reaction.rate)

        result=""

        for r in self.data.reactions:
            reaction = r.reaction
            name = rate_template.substitute(name=reaction.name)
            result += declaration_template.substitute(
                name=name,
                rate=reaction.rate)

        return result

    def reactivity(self):
        reactivity_body_template = Template(self.template.reactivity.body)
        reactivity_call_template = Template(self.template.reactivity.call)
        reactivity_name_template = Template(self.template.reaction.reactivity.name)

        result=""
        plus=""
        for r in self.data.reactions:
            reaction = r.reaction
            name = reactivity_name_template.substitute(name=reaction.name)
            call = reactivity_call_template.substitute(method=name)
            result += plus + call
            plus = " + \n     "

        return reactivity_body_template.substitute(reactivity=result)



if __name__ == '__main__':
    arguments = docopt(__doc__, version="v0.1")
    #    print(arguments)

    model_stream = open(arguments['<model>'])
    model_data = load(model_stream, Loader=Loader)
    output = dump(model_data, Dumper=Dumper)

    template_stream = open(arguments['<template>'])
    template_data = load(template_stream, Loader=Loader)
    output = dump(template_data, Dumper=Dumper)

    model = Model(template_data, model_data)

    print model.header()
    print model.model()
    print model.reaction_reactivity()
    print model.diffusion_rate()
    print model.diffusion_step()
    print model.declaration()
    print model.reactivity()