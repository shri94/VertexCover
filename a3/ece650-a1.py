#!/usr/bin/env python3
import sys
from collections import OrderedDict
from graph import Graph
from intersect import point, line, street as Street, intersect
import re

#Global Variables
streets = OrderedDict()

def parse(line):
    """
    Validates input.
    Gets command and associated information

    Args:
        line: input string entered by user

    Outputs:
        Stdout/Stderr

    Returns:
        None

    Raises:
        None
    """
    # removing trailing \n for line
    line = line[:-1]

    if line == "gg":
        # Prints the graph datastructure with existing vertices and edges
        build_graph(True)
    elif line.startswith("rm"):
        # Removes street
        pattern = "^rm\s(\"([a-zA-Z]+\s?)*[a-zA-Z]+\")"
        p = re.compile(pattern)
        result = p.fullmatch(line)
        if result:
            street_name = result.group(1)
            street_name = street_name.lower()
            if street_name not in streets:
                print("Error: Invalid Input. ", street_name, "does not exits")
            else:
                del streets[street_name]
                build_graph()
        else:
            print("Error: Invalid Input. Formatting Error. rm didnt work")
    elif line.startswith("add"):
        # adds a new valid street
        pattern = "^add\s(\"([a-zA-Z]+\s?)*[a-zA-Z]+\")\s((\(-?\d+,-?\d+\)\s)+(\(-?\d+,-?\d+\)))"
        p = re.compile(pattern)
        result = p.fullmatch(line)

        if result:
            street_name = result.group(1)
            coordinates_str = result.group(3)

            # check for neg 0
            neg_zero_pattern = "-0+(,|\))"
            neg_zeros = re.findall(neg_zero_pattern, coordinates_str)
            if neg_zeros:
                print("Error: Invalid Input. 0 can't be input as negative")
                return

            # store as lowercase name
            street_name = street_name.lower()
            if street_name in streets:
                print("Error: Invalid Input. Street: ", street_name, "already exits")
            else:
                points = convert_coordinate_str_to_points(coordinates_str)
                streets[street_name] = Street(street_name, points)
                build_graph()
        else:
            print("Error: Invalid Input. Formatting Error. add didnt work")
    elif line.startswith("mod"):
        # modifies an existing street
        pattern = "^mod\s(\"([a-zA-Z]+\s?)*[a-zA-Z]+\")\s((\(-?\d+,-?\d+\)\s)+(\(-?\d+,-?\d+\)))"
        p = re.compile(pattern)
        result = p.fullmatch(line)
        if result:
            street_name = result.group(1)
            coordinates_str = result.group(3)

            # check for neg 0
            neg_zero_pattern = "-0+(,|\))"
            neg_zeros = re.findall(neg_zero_pattern, coordinates_str)
            if neg_zeros:
                print("Error: Invalid Input. 0 can't be input as negative")
                return

            street_name = street_name.lower()
            if street_name not in streets:
                print("Error: Invalid Input. Street: ", street_name, "does not exits")
            else:
                points = convert_coordinate_str_to_points(coordinates_str)
                streets[street_name].points = points
                build_graph()
        else:
            print("Error: Invalid Input. Formatting Error. mod didnt work")
    else:
        print("Error: Invalid Input. Make sure to either add, rm, mod, or gg" , flush=True)

def convert_coordinate_str_to_points(coordinates_str):
    """
    Converts a string with coordinates to Point object

    Args:
        coordinates_str: coordinate string extracted from input

    Outputs:
        Stdout/Stderr

    Returns:
        li_of_points: a list of Points

    Raises:
        None
    """
    if coordinates_str == '':
        return []

    pattern = "\((-?\d+),(-?\d+)\)"
    coordinates_list = re.findall(pattern, coordinates_str)

    li_of_points = []
    for co in coordinates_list:
        p = point(co[0], co[1])
        li_of_points.append(p)
    return(li_of_points)

def build_graph(print_the_graph = False):
    """
    Creates/modifies a graph
    Invokes print_graph if required

    Args:
        print_the_graph: Boolean flag to print the graph

    Outputs:
        Stdout/Stderr

    Returns:
        None

    Raises:
        None
    """
    if len(streets) < 2:
        # There cant be any v or e in this case
        gph = Graph()
    else:
        lines_with_intersections = find_intersections()
        vertices, edges = get_vertices_and_edges(lines_with_intersections)
        gph = Graph(vertices, edges)
    if print_the_graph:
        gph.print_graph()
        # Instead of printing, pass the output to a2.cpp
        # and then clear the graph
        # streets.clear()

def find_intersections():
    """
    Finds the intersections on all lines in the existing streets(global)

    Args:
        None

    Outputs:
        Stdout/Stderr

    Returns:
        lines_with_intersections - dictionary:
                                   key: Line object
                                   values = set of intersections on the line

    Raises:
        None
    """
    if len(streets) < 1:
        return set()

    #int_point_for_lines = set()
    lines_with_intersections = dict()
    i=0
    lines_in_street = []

    # gets all the lines in a street
    # in the form: [{set of lines on street1}, {...s2}..]
    for key, val in streets.items():
        lines_in_street.append(val.get_lines())

    number_of_streets = len(lines_in_street)

    # Embarassing time complexity but gets the job done..
    # Goes through 1 street a time
    # Compares it with other streets
    for i in range(number_of_streets-1):
        if i+1 > number_of_streets-1:
            break
        for j in range(i+1, number_of_streets):
            # fetches the lines for both the streets being compared
            s1_lines = lines_in_street[i]
            s2_lines = lines_in_street[j]
            for line1 in s1_lines:
                for line2 in s2_lines:
                    # checks for intersections for all possiblities
                    int_point = intersect(line1, line2)
                    if int_point:
                        # if intersection is found using these 2 lines,
                        # updates both line object with intersection
                        line1.add_intersections(int_point.co)
                        line2.add_intersections(int_point.co)

    # goes through lines on each street
    # updates lines_with_intersections in the return format
    for lines in lines_in_street:
        for line in lines:
            lines_with_intersections[line] = line.intersections

    return lines_with_intersections

def get_vertices_and_edges(lines_with_intersections):
    """
    Determines the vertices and edges using lines_with_intersections

    Args:
        lines_with_intersections - dictionary:
                                   key: Line object
                                   values = set of intersections on the line

    Outputs:
        Stdout/Stderr

    Returns:
        vertices - set of coordinate points
        edges - set of tuples with coordinate points

    Raises:
        None
    """
    if not lines_with_intersections:
        return None, None

    vertices = set()
    edges = set()
    for line, intersections in lines_with_intersections.items():
        line_src_co = line.src.co
        line_dst_co = line.dst.co

        all_points_on_line = [line_src_co, line_dst_co] + list(intersections)
        # sort by y
        all_points_on_line = sorted(all_points_on_line, key=lambda x:x[1])
        # sort by x
        all_points_on_line = sorted(all_points_on_line, key=lambda x:x[0])

        if len(all_points_on_line) > 2:
            vertices.update({pt for pt in all_points_on_line})
            len_all_points_on_line = len(all_points_on_line)
            i = 0
            for i in range(len_all_points_on_line):
                if i == len_all_points_on_line -1:
                    break
                edges.add((all_points_on_line[i],all_points_on_line[i+1]))
                i = i+1

    return vertices, edges

def main():
    """
    Standard main.

    Args:
        None

    Outputs:
        Stdout/Stderr

    Returns:
        None

    Raises:
        None
    """
    while True:
        line = sys.stdin.readline()
        if line == "":
            break
        # parse input
        parse(line)
    # return exit code 0 on successful termination
    sys.exit(0)

if __name__ == "__main__":
    main()
