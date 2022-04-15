def pp(x):
    """Returns a pretty-print string representation of a number.
       A float number is represented by an integer, if it is whole,
       and up to two decimal places if it isn't
       Func provided by prof Vijay Ganes
    """
    if isinstance(x, float):
        if x.is_integer():
            return str(int(x))
        else:
            return "{0:.2f}".format(x)
    return str(x)

class Graph:
    """
    This class is used to create the graph data structure
    """
    def __init__(self, V=set(), E=set()):
        """
        Constructor to set basic empty graph

        Args:
            V - Set of Vertices
            E - Set of Edges (tuple of coordinate tuples)

        Outputs:
            None

        Returns:
            None

        Raises:
            None
        """
        self._edge_set = set()
        self._vertices_dict = dict()
        for vertex in set(V):
            self.add_vertex(vertex)
        for edge in E:
            self.add_edge(edge)

    def add_vertex(self, v):
        """
        Add a new vertex in the vertices dict
        Create a unique name for the point

        Args:
            v - vertex eg. (1.0, 4.0)

        Outputs:
            None

        Returns:
            None

        Raises:
            None
        """
        if v not in self._vertices_dict.values():
            vname = "x" + str(int(v[0]))+ "y" + str(int(v[1]))
            self._vertices_dict[v] = vname

    def add_edge(self, e):
        """
        Add a new edge in the vertices dict

        Args:
            e - vertex eg. ((1.0, 4.0), (2.0, 2.0))

        Outputs:
            None

        Returns:
            None

        Raises:
            None
        """
        p1 = e[0]
        p2 = e[1]
        if p1 in self._vertices_dict \
            and p2 in self._vertices_dict:
            self._edge_set.add((self._vertices_dict[p1], self._vertices_dict[p2]))

    def print_graph(self):
        """
        Prints a graph with v and e

        Args:
            None

        Outputs:
            None

        Returns:
            None

        Raises:
            None
        """
        print("V: {")
        for v, v_name in self._vertices_dict.items():
            s = "  "+v_name + ": (" + pp(v[0]) + "," + pp(v[1]) +")"
            print(s)
        print("}")

        print("E: {")

        list_e = list(self._edge_set)
        num_of_edges = len(list_e)
        for i in range(num_of_edges):
            st = "  <" + list_e[i][0] + "," \
                + list_e[i][1] + ">"
            if i != num_of_edges -1:
                st += ","
            print(st)
        print("}")

    def __repr__(self):
        rs = "Vertices: "+ str(self._vertices_dict)
        rs += "\nEdges: "+ str(self._edge_set)
        return(rs)
