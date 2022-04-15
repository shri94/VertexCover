"""
HELPER MODULE: Provided by Dr. Vijay Ganesh used as base
found @ https://git.uwaterloo.ca/ece650-f2021/python-examples
Modified and updated by Shridhar Suryanarayan
"""

def pp(x):
    """Returns a pretty-print string representation of a number.
       A float number is represented by an integer, if it is whole,
       and up to two decimal places if it isn't
    """
    if isinstance(x, float):
        if x.is_integer():
            return str(int(x))
        else:
            return "{0:.2f}".format(x)
    return str(x)

class point(object):
    """A point in a two dimensional space"""
    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)

    def __repr__(self):
        return '(' + pp(self.x) + ', ' + pp(self.y) + ')'

    @property
    def co(self):
        return(((self.x),(self.y)))


class line(object):
    """A line between two points"""
    def __init__(self, src, dst):
        self.src = src
        self.dst = dst
        self._intersections = set()

    @property
    def intersections(self):
        return self._intersections

    def add_intersections(self, value):
        if value == self.src.co or value == self.dst.co:
            return
        else:
            self._intersections.add(value)

    def remove_intersections(self, value):
        if value in self._intersections:
            self._intersections.remove(value)

    def __repr__(self):
        return '['+ str(self.src) + '-->' + str(self.dst) + ']'


class street(object):
    """Street class that contains a name and points forming lines"""
    def __init__(self, name=None, points=[]):
        self._name = name
        self._points = list(points)

    @property
    def name(self):
        if not self._name:
            return "Error: Unnamed Street"
        return self._name

    @name.setter
    def name(self, value):
        self._name = value

    @name.deleter
    def name(self):
        del self._name

    @property
    def points(self):
        if not self._points:
            return "Error: No points provided"
        return self._points

    @points.setter
    def points(self, value):
        self._points = value

    def get_lines(self):
        if not self._points:
            return "Error: No points provided"
        lines = set()
        _i = 0
        for _i  in range(len(self._points)-1):
            l = line(self._points[_i],self._points[_i +1])
            lines.add(l)
        return lines


def intersect(l1, l2):
    """Returns a point at which two lines intersect
    created using information from : https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
    """

    x1, y1 = l1.src.x, l1.src.y
    x2, y2 = l1.dst.x, l1.dst.y

    x3, y3 = l2.src.x, l2.src.y
    x4, y4 = l2.dst.x, l2.dst.y

    xnum = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4))
    xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))

    try:
        xcoor =  xnum / xden
    except ZeroDivisionError:
        return None

    ynum = (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)
    yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4)
    try:
        ycoor = ynum / yden
    except ZeroDivisionError:
        return None

    tnum = ((x1-x3)*(y3-y4) - (y1-y3)*(x3-x4))
    tden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
    try:
        t = tnum / tden
    except ZeroDivisionError:
        t = float('inf')

    unum = ((x1-x3)*(y1-y2) - (y1-y3)*(x1-x2))
    uden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
    try:
        u = unum / uden
    except ZeroDivisionError:
        u = float('inf')

    if 0.0 <= t <= 1.0 and 0.0 <= u <= 1.0:
        int_point = point(xcoor, ycoor)
        return int_point
    else:
        return None
