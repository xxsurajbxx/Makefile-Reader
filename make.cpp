#include <iostream>
#include <fstream>

using namespace std;

int main(int NumberofArgs, char* args[]) {    bool debug=false;
    //open the file
    ifstream makefile;    if(NumberofArgs<=3) {
        if(NumberofArgs==1) {makefile.open("makefile");}        else if(NumberofArgs>=2) {makefile.open(args[1]);}        if(NumberofArgs==3&&args[2][0]=='-'&&args[2][1]=='d') {debug=true;}
    }    else {cout << "Error: too many arguments" << endl; return 0;}
    bool errors_exist=false;
    if(makefile.is_open()) {
        //extract each line from the makefile. there can be a total of 100 lines in the file
        int numofLines=0;
        string line;
        string lines[100];
        while(getline(makefile, line)) {
            lines[numofLines++]=line;
        }
        //extract each line with a target from the file
        string targets[numofLines];
        int numofTargets=0;
        for(int i=0; i<numofLines; i++) {
            if(lines[i][0]!='\t'&&lines[i][0]!='\0') {targets[numofTargets++]=lines[i];}
        }
        //extract each dependancy from each target with a maximum of 10 dependencies per each target
        string dependencies[numofTargets][10];
        int numofDependencies=0;
        int dependenciesPerTarget[numofTargets];
        for(int i=0; i<numofTargets; i++) {
            int b=targets[i].find(' ');
            label:
            int a=targets[i].find(' ', b+1);
            if(a<0) {a=targets[i].length();}
            dependencies[i][numofDependencies++]=targets[i].substr(b+1, a-(b+1));
            if(targets[i][a]=='\0') {dependenciesPerTarget[i]=numofDependencies; numofDependencies=0;}
            else {b=a; goto label;}
        }
        //shorten each target line so it only has the actual target, without the dependancy
        for(int i=0; i<numofTargets; i++) {
            string temp;
            for(int x=0; x<targets[i].find(':'); x++) {
                temp+=targets[i][x];
            }
            targets[i]=temp;
        }        //debug information        if(debug) {            cout << "File Contents:" << endl;            for(int i=0; i<numofLines; i++) {                cout << lines[i] << endl;            }            cout << endl;            cout << "Number of lines: " << numofLines << endl;            cout << "Number of targets: " << numofTargets << endl;            for(int i=0; i<numofTargets; i++) {                cout << targets[i] << " dependencies:" << endl;                cout << "\tCount: " << dependenciesPerTarget[i] << endl;                for(int x=0; x<dependenciesPerTarget[i]; x++) {                    cout << '\t' << dependencies[i][x] << endl;                }                cout << endl;            }        }
        //extract each command from the file. can hold, max, 100 commands per target.
        string commands[numofTargets][100];
        int commandsPerTarget[numofTargets];
        int numofCommands=0;
        int counter=0;
        for(int i=0; i<numofLines; i++) {
            if(lines[i][0]=='\t') {
                commands[counter][numofCommands++]=lines[i];
            }
            else if(i!=0&&lines[i][0]!='\0') {
                commandsPerTarget[counter++] = numofCommands;
                numofCommands=0;
            }
        }
        commandsPerTarget[counter] = numofCommands;
        //check if there are any errors acessing the files
        for(int x=0; x<numofTargets; x++) {
            for(int y=0; y<dependenciesPerTarget[x]; y++) {
                bool is_target=false;
                for(int i=0; i<numofTargets; i++) {
                    if(dependencies[x][y]==targets[i]) {is_target=true;}
                }
                if(!is_target) {
                    ifstream file;
                    file.open(dependencies[x][y]);
                    if(!file.good()) {cout << "Error: file " << dependencies[x][y] << " couldn't be found" << endl; errors_exist=true;}
                    else {file.close();}
                }
            }
        }
        //if there are errors then end the program
        if(errors_exist) {makefile.close(); return 0;}
        //first find the order in which to run each target then execute each command
        bool checklist[numofTargets]={false};
        for(int i=0; i<numofTargets; i++) {
            for(int x=0; x<numofTargets; x++) {
                if(!checklist[x]) {
                    bool is_target=false;
                    for(int y=0; y<dependenciesPerTarget[x]; y++) {
                        for(int a=0; a<numofTargets; a++) {
                            if(targets[a]==dependencies[x][y]&&!checklist[a]) {
                                //cout << targets[a] << endl;
                                is_target=true;
                                a=numofTargets;
                                y=dependenciesPerTarget[x];
                            }
                        }
                    }
                    if(!is_target) {
                        for(int h=0; h<commandsPerTarget[x]; h++) {
                            system(commands[x][h].c_str());
                        }
                        checklist[x]=true;
                    }
                }
            }
        }
        makefile.close();
    }
    else {cout << "Error: file couldn't be opened" << endl;return 0;}
}