#   Visualize Soultion script
#
#   Presents a virtual representation of the input and respective solution provided 
#
#   Usage: python visualize_solution.py N testcase_input_file [solution_file] [output_path]
#               N - id for the image, file saved with name 'testN.png' in output_path folder, if provided
#               testcase_input_file - path to file with input testcase
#               solution_file - [optional] path to file with the solution, in the format of 
#                                    0 0 0 0 0 0 0 (one number per antena location in the input 
#                                                    representing the type of antenna to install
#                                                    in that location: 0 - no antenna, 
#                                                                      1<=k<=t -antenna of type k)
#               output_path - [optional] path to place the generated images. If not provided, images are stored
#                                   in the current path
#
#   Created by Marco Simoes
import matplotlib as mpl
import os
if os.environ.get('DISPLAY','') == '':
    print('No display found. Using non-interactive Agg backend')
    mpl.use('Agg')
import matplotlib.pyplot as plt
import sys


# Creates and saves a plot with the location of the viewers (in blue) 
# and the candidate locations of antennas (in green). If a solution is 
# provided it plots a red circle to represent the coverage of the antenna
# and the respective position.
def plot_test_case(tid, viewers_pos, ant_pos, ant_types, sol=None, filepath='.' ):
    fig, ax = plt.subplots()

    # make a small blue circle for each viewer position
    for pos in viewers_pos:
        circle = plt.Circle((pos[0], pos[1]), 0.2, color='b')
        ax.add_artist(circle)

    # make a small green circle for each candidate antenna position
    for pos in ant_pos:
        circle = plt.Circle((pos[0], pos[1]), 0.2, color='g')
        ax.add_artist(circle)
    

    # define plot limits 
    lst = viewers_pos
    lst.extend(ant_pos)

    maxxy = list(map(max, zip(*lst)))
    minxy = list(map(min, zip(*lst)))

    ax.set_xlim(minxy[0]-ant_types[-1][0], maxxy[0]+ant_types[-1][0])
    ax.set_ylim(minxy[1]-ant_types[-1][0], maxxy[1]+ant_types[-1][0])


    # if solution is provided, include them in the image
    sol_ext = ''
    if not sol == None:
        for i in range(len(sol)):
            if sol[i] > 0:

                # make a circle with radius depending on the type of antenna
                circle = plt.Circle((ant_pos[i][0], ant_pos[i][1]), ant_types[sol[i]-1][0], color='r', fill=False)
                ax.add_artist(circle)

                # overlap green circle with red one in the antenna position
                circle = plt.Circle((ant_pos[i][0], ant_pos[i][1]), 0.2, color='r')
                ax.add_artist(circle)

        sol_ext = '_sol'

    
    # Show figure -> Uncomment to preview image
    # plt.show()
    
    # save file
    plt.savefig('%s/test%d%s.png'% (filepath, tid, sol_ext))


# Reads solution from file, expecting one integer for each 
# antenna candidate position, separeted by a space character,
# matching the order of candidate antenna positions in the input.
# Each number corresponds to the type of antena to place at that position
# being 0 no antenna and t (> 0) the type of antenna with the same order
# in the input.
# Ex.: 0 1 0 3
# first candidate position: no antenna
# second candidate position: antenna of the first type as defined by the input
# third candidate position: no antenna
# fourth candidate position: antenna of the third type as defined by the input
def read_solution(name):
    with open(name, 'r') as f:
        line = f.readline()
        parts = line.split()
        return [int(x) for x in parts]


# Read the input of a test case containing the viewers_positions, 
# candidate antennas positions and types of antennas
def read_input(name):
    with open(name, 'r') as f:
        
        # read viewers positions
        line = f.readline()
        n = int(line)
        viewers_pos = []
        for i in range(n):
            parts = f.readline().split()
            viewers_pos.append( (int(parts[0]), int(parts[1])) )

        # read candidate antennas positions
        line = f.readline()
        n = int(line)
        ant_pos = []
        for i in range(n):
            parts = f.readline().split()
            ant_pos.append( (int(parts[0]), int(parts[1])) )

        # read antennas types
        line = f.readline()
        n = int(line)
        ant_types = []
        for i in range(n):
            parts = f.readline().split()
            ant_types.append( (int(parts[0]), int(parts[1])) )
        
        return (viewers_pos, ant_pos, ant_types)


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('Usage: visualize_solution.py N testX.in [solutionX.out] [outputpath]')
        exit()

    # read input
    (vp, ap, tp) = read_input(sys.argv[2])

    # if provided, read solution
    sol = None
    if (len(sys.argv)>3):
        sol = read_solution(sys.argv[3])

    # if provided, use output path
    outpath = '.'
    if (len(sys.argv)>4):
        outpath = sys.argv[4]

    # call function
    plot_test_case(int(sys.argv[1]), vp, ap, tp, sol, outpath)