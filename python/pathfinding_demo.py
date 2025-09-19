#!/usr/bin/env python
# coding: utf-8

#
# Imports
#

import matplotlib.pyplot as plt
import numpy as np
import time
from typing import Protocol, Optional

#
# Type and interfaces definition
#

type Point2D = tuple[int, int] # tuple(x, y)
type Path = list[Point2D]
type ElapsedTime_ns = int # nanoseconds
type VisitedNodeCount = int

class Map:
    """
    2D map consisting of cells with given cost
    """
    # array not defined as private, as plotting utilities work with it directly
    array: np.array
    _visited_nodes: int

    def __init__(self, width: int, height: int) -> None:
        assert width > 0
        assert height > 0
        rows = height
        cols = width
        self.array = np.zeros((rows, cols), dtype=np.float64)
        self._visited_nodes = 0

    def Randomize(self, low: float = 0.0, high: float = 1.0) -> None:
        self.array = np.random.uniform(low, high, self.array.shape)

    def IsPointValid(self, point: Point2D) -> bool:
        x, y = point
        y_max, x_max = self.array.shape
        x_in_bounds = (0 <= x < x_max) 
        y_in_bounds = (0 <= y < y_max) 
        return x_in_bounds and y_in_bounds
    
    def GetNeighbours(self, center_point: Point2D) -> list[Point2D]:
        """
        Get list of neighboring points (without actually visiting them)
        """
        points: list[Point2D] = []
        x_center, y_center = center_point
        for x in range(-1,2):
            for y in range(-1,2):
                if x == 0 and y == 0:
                    continue
                p = (x + x_center, y + y_center)
                if self.IsPointValid(p):
                    points.append(p)
        return points
    
    def ResetVisitedCount(self) -> None:
        self._visited_nodes = 0

    def GetVisitedCount(self) -> int:
        return self._visited_nodes

    def Visit(self, point: Point2D) -> float:
        """
        Visit the node, returning its cost
        """
        if not self.IsPointValid(point):
            raise ValueError("Point out of bounds")
        self._visited_nodes += 1
        x, y = point
        row, col = y, x
        return self.array[(row, col)]


class PathFinder(Protocol):
    def SetMap(m: Map) -> None:
        ...

    def CalculatePath(start: Point2D, end: Point2D) -> Path:
        """
        Calculate path on a given map.
        Note: map must be set first using SetMap (or using constructor)
        """

    def GetStats() -> (ElapsedTime_ns, VisitedNodeCount):
        """
        Return performance stats for the last calculation:
        - elapsed time in nanoseconds,
        - number of visited nodes during search
        """

#
# Drawing utilities
#

class Visualizer:
    _axes: Optional[plt.Axes]
    _cmap: plt.Colormap
    _cmap_counter: int

    def __init__(self):
        self._axes = None
        self._cmap = plt.get_cmap('tab10')
        self._cmap_counter = 0

    def DrawMap(self, m: Map):
        M, N = m.array.shape
        _, ax = plt.subplots()
        ax.imshow(m.array, cmap='gist_earth', origin='lower', interpolation='none')
        self._axes = ax

    def DrawPath(self, path: Path, label: str = "Path"):

        """
        Draw path on a map. Note that DrawMap has to be called first
        """
        assert self._axes is not None, "DrawMap must be called first"
        xs, ys = zip(*path)
        color = self._cmap(self._cmap_counter)
        self._cmap_counter += 1
        self._axes.plot(xs, ys, 'o-', color=color, label=label)
        self._axes.plot(xs[0],  ys[0],  'o', color='lime',  markersize=8)  # starting point
        self._axes.plot(xs[-1], ys[-1], 'o', color='magenta', markersize=8)  # end point

#
# Method: depth-first search
#

class DFS:

    name = "Depth First Search"
    _map: Optional[Map]
    _elapsed_time_ns: int
    _visited_node_count: int

    def __init__(self) -> None:
        self._map = None

    def SetMap(self, m: Map) -> None:
        self._map = m

    def CalculatePath(self, start: Point2D, end: Point2D) -> Path:
        assert self._map is not None, "SetMap must be called first"
        self._map.ResetVisitedCount()
        start_time = time.perf_counter_ns()

        res = self._CalculatePath(start, end)
        print(f"{res=}")

        stop_time = time.perf_counter_ns()
        self._elapsed_time_ns = stop_time - start_time
        self._visited_node_count = self._map.GetVisitedCount()
        return res

    def GetStats(self) -> (ElapsedTime_ns, VisitedNodeCount):
        return self._elapsed_time_ns, self._visited_node_count

    def _CalculatePath(self, 
                       point: Point2D,
                       end_point: Point2D,
                       path: Optional[list[Point2D]] = None,
                       visited: Optional[set[Point2D]] = None) -> Optional[Path]:
        """
        Find (any) path, not guaranteed to be optimal (and it most probably won't be)
        """
        if visited is None:
            visited = set()
        if path is None:
            path = list()
        # We don't need to know cost in this case, but we still want to track
        # how many nodes we've visited
        _ = self._map.Visit(point) 
        # we keep visited nodes in separate list and set,
        # as membership check is faster for set than for list,
        # but set is not ordered
        visited.add(point)
        path.append(point)
        if point == end_point:
            return path 
        for neighbor in self._map.GetNeighbours(point):
            if neighbor not in visited:
                res = self._CalculatePath(neighbor, end_point, path, visited)
                if res:
                    return res
        return None

#
# Calculate paths using various methods and visualize them
#

def main():
    # Define the map and start/stop points
    m = Map(15,10)
    m.Randomize()
    starting_point: Point2D = (0,9)
    end_point: Point2D = (14,1)

    path_finder_classes: list[PathFinder] = {
        DFS,
    }

    v = Visualizer()
    v.DrawMap(m)

    for pfc in path_finder_classes:
        path_finder = pfc()
        path_finder.SetMap(m)
        path = path_finder.CalculatePath(starting_point, end_point)
        elapsed_time, visited_nodes = path_finder.GetStats()
        print(f"{path_finder.name:22}: took {elapsed_time/1e6} ms, visited {visited_nodes} nodes")
        v.DrawPath(path)

    plt.show()

if __name__ == "__main__":
    main()
