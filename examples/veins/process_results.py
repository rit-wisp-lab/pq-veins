import os

os.chdir('results')
for filename in os.listdir(os.getcwd()):
    if filename.endswith('.sca'):
        fname = filename.replace('#','\\#')
        resultsName = fname.replace('.sca','.csv')
        os.system(f'scavetool x {fname} -o {resultsName}')
