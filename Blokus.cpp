#include<SDL/SDL.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<math.h>

#define PI 3.14159265
using namespace std;
//MODES:
//0=select
//1=locate
bool locateMode=0,turnLeft=0;
float animation_Speed=0.01;
float X_LOCATE=0.0,Y_LOCATE=0.0;
//ROTATION STATE - 0=0 degrees, 1= 90degrees 2=180, 3= 270
// R_STATES - storage of  rotation states for figures on the board
//MARGINS - margins for a figure for the [0]- bottom [1] - right, [2]-top,[3]-left
int ROTATION_STATE=0,R_STATES[42],MARGINS[4];
float myDir = 0;
float BU_X=float(0.07+float(3/7.0)),BU_Y=float(0.05+float(3/7.0));
float BMU_X=float(1/3.53),BMU_Y=float(1/3.53);
float board_figures_x[42],board_figures_y[42];
bool keyPressed_R=0,
    keyPressed_L=0,
    keyPressed_U=0,
    keyPressed_D=0;
float PosX=0,PosY=0,posZ=0;
float cameraX=5.0,cameraY=0.0,cameraZ=5.0;
float selectedAngleCurrent=0.0;
int selectedDirection=1;
        float initial_position=float(1/7.5);

float toRads(float angleinDegrees){

    return (PI*angleinDegrees)/180;
}
float temp_change=0.0,temp_change2=0.0;
int cubesInFigureDown(int figure_num){
    if(figure_num>21 || figure_num<1){
        return 0;
    }
    switch(figure_num){
        case 1: return 0;
        case 2: return 1;
        case 4: return 1;
        case 5: return 3;
        case 8: return 1;
        case 9: return 1;
        case 10: return 4;
        case 11: return 3;
        case 12: return 3;
        case 15: return 3;
        default:return 2;
    }
}
int cubesInFigureLeft(int figure_num){
    switch(figure_num){
        case 6: return 1;
        case 11: return 1;
        case 12: return 1;
        case 13: return 1;
        case 14: return 1;
        case 16: return 1;
        case 21: return 1;
        default:return 0;
    }
}
int cubesInFigureRight(int figure_num){
    switch(figure_num){
        case 4: return 1;
        case 7: return 1;
        case 8: return 1;
        case 9: return 2;
        case 15: return 1;
        case 16: return 1;
        case 17: return 2;
        case 18: return 2;
        case 19: return 2;
        case 20: return 2;
        case 21: return 1;
        default:return 0;
    }
}
void init(){
    glClearColor(83.0/255.0, 172.0/255.0, 172.0/255.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    for(int i=0;i<42;i++){
        board_figures_x[i]=board_figures_y[i]=R_STATES[i]=0;
    }
    for(int i=0;i<4;i++){
        MARGINS[i]=0;
    }
    gluPerspective(45,128.0/72.0,1.0,500.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    //alpha channel
//    glEnable(GL_BLEND);
  //  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);
    }
int totalCubes(int figure_num){
    if(figure_num<4){
        return figure_num;
    }
    if(figure_num==4)return 3;
    if(figure_num>9){
        return 5;
    }
    return 4;
}
//gameBox
class gameBox{
    //1 means available 0 means not

    //board with boxes shown as taken places or not
    //0 means not taken
    //1 means taken by the right player
    //2 means taken by the left player
    unsigned int board2[196];
    bool left_player[21]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    bool right_player[21]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    unsigned int selected[2]={1,1};
    public:
        bool board[196];
        gameBox(){
            for(int i=0;i<196;i++){
                board[i]=1;
                board2[i]=0;
            }
            board2[0]=3;
            board2[195]=4;
         }
        //get-set
        bool getBoard(int x,int y){
            if(x>13 || y>13||x<0 || y<0){
                return 0;
            }
            return board[x+y*14];
        }
        bool getLeftAvail(int figure_num){
            return left_player[figure_num];
        }
        bool getRightAvail(int figure_num){
            return right_player[figure_num];
        }
        bool setLeftAvail(int figure_num,int val){
            if(figure_num<14 && figure_num>=0){
            left_player[figure_num]=val;
            return 1;}
            return 0;
        }
        bool setRightAvail(int figure_num,int val){
            if(figure_num<14 && figure_num>=0){
            right_player[figure_num]=val;
            return 1;}
            return 0;
        }
        int getSelected(bool right){
            return selected[right];
        }
        bool setSelected(bool right,int value){
            if(value<22 && value>0){
            selected[right]=value;
            return 1;}
            return 0;
        }
        //availability verifier: 1- means available, 0 means not
        bool verifier(int figure_num,int x,int y){
            y+=7;x+=7;
                int position_y[5]={14*y,14*(y-1),14*(y-2),14*(y-3),14*(y-4)},position_x[4]={x-1,x,x+1,x+2};
                switch(ROTATION_STATE){
                    case 1:
                            for(int i=1;i<5;i++){
                                position_y[i]+=(i)*14+i;
                            }
                            position_x[0]-=13;
                            position_x[2]+=13;
                            position_x[3]+=26;
                        break;
                    case 2:
                        for(int i=1;i<5;i++){
                            position_y[i]+=(i)*28;
                        }
                        position_x[0]+=2;
                        position_x[2]-=2;
                        position_x[3]-=3;
                        break;
                    case 3:
                        for(int i=1;i<5;i++){
                            position_y[i]+=(i)*14-i;
                        }
                        position_x[0]+=15;
                        position_x[2]-=15;
                        position_x[3]-=30;
                        break;
                }
                switch(figure_num){
                //implement movement limits in graphics !!!
                case 1:if(board[position_y[0]+position_x[1]]) {
                        board[position_y[0]+position_x[1]] = 0;
                        board2[position_y[0]+position_x[1]] = turnLeft+3;
                        return 1;}
                        break;
                case 2:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]]){
                     board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = 0;
                     board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = turnLeft+3;
                     return 1;}
                    break;
                case 3:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]]){
                            board[position_y[0]+position_x[1]] = 0;board[position_y[1]+position_x[1]] =0; board[position_y[2]+position_x[1]] = 0;
                            board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = turnLeft +3;
                            return 1;}
                            break;
                case 4:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[1]+position_x[2]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[1]+position_x[2]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[1]+position_x[2]] = turnLeft+3;
                        return 1;}
                        break;
                case 5:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[3]+position_x[1]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[3]+position_x[1]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[3]+position_x[1]] = turnLeft+3;
                        return 1;}
                        break;
                case 6:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[0]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[2]+position_x[0]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[2]+position_x[0]] = turnLeft +3 ;
                        return 1;}
                        break;
                case 7:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[1]+position_x[2]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[1]+position_x[2]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[1]+position_x[2]] = turnLeft+3;
                        return 1;}
                        break;
                case 8:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[1]+position_x[2]] && board[position_y[0]+position_x[2]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[1]+position_x[2]] = board[position_y[0]+position_x[2]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[1]+position_x[2]] = board2[position_y[0]+position_x[2]] = turnLeft+3;
                        return 1;}
                        break;
                case 9:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[2]] && board[position_y[0]+position_x[2]] && board[position_y[1]+position_x[3]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[2]] = board[position_y[0]+position_x[2]] = board[position_y[1]+position_x[3]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[2]] = board2[position_y[0]+position_x[2]] = board2[position_y[1]+position_x[3]] = turnLeft+3;
                        return 1;}
                         break;
                case 10:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[3]+position_x[1]] && board[position_y[4]+position_x[1]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[3]+position_x[1]] = board[position_y[4]+position_x[1]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[3]+position_x[1]] = board2[position_y[4]+position_x[1]] = turnLeft+3;
                        return 1;}
                        break;
                case 11:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[3]+position_x[1]] && board[position_y[3]+position_x[0]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[3]+position_x[1]] = board[position_y[3]+position_x[0]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[3]+position_x[1]] = board2[position_y[3]+position_x[0]] = turnLeft+3;
                        return 1;}
                         break;
                case 12:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[0]] && board[position_y[3]+position_x[0]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[2]+position_x[0]] = board[position_y[3]+position_x[0]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[2]+position_x[0]] = board2[position_y[3]+position_x[0]] = turnLeft+3;
                        return 1;}
                         break;
                case 13:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[0]] && board[position_y[1]+position_x[0]] ){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[2]+position_x[0]] = board[position_y[1]+position_x[0]] = 0;
                        board2[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[2]+position_x[0]] = board2[position_y[1]+position_x[0]] = turnLeft+3;
                        return 1;}
                         break;
                case 14:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[0]] && board[position_y[0]+position_x[0]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[2]+position_x[0]] = board[position_y[0]+position_x[0]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[2]+position_x[0]] = board2[position_y[0]+position_x[0]] = turnLeft+3;
                        return 1;}
                         break;
                case 15:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[3]+position_x[1]] && board[position_y[1]+position_x[2]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[3]+position_x[1]] = board[position_y[1]+position_x[2]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[3]+position_x[1]] = board2[position_y[1]+position_x[2]] = turnLeft+3;
                        return 1;}
                         break;
                case 16:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[0]] && board[position_y[2]+position_x[2]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[2]+position_x[0]] = board[position_y[2]+position_x[2]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[2]+position_x[0]] = board2[position_y[2]+position_x[2]] = turnLeft+3;
                        return 1;}
                         break;
                case 17:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[2]] && board[position_y[2]+position_x[3]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[2]+position_x[1]] = board[position_y[2]+position_x[2]] = board[position_y[2]+position_x[3]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[2]+position_x[1]] = board2[position_y[2]+position_x[2]] = board2[position_y[2]+position_x[3]] = turnLeft+3;
                        return 1;}
                         break;
                case 18:if(board[position_y[0]+position_x[1]] && board[position_y[0]+position_x[2]] && board[position_y[1]+position_x[2]] && board[position_y[1]+position_x[3]] && board[position_y[2]+position_x[3]]){
                        board[position_y[0]+position_x[1]] = board[position_y[0]+position_x[2]] = board[position_y[1]+position_x[2]] = board[position_y[1]+position_x[3]] = board[position_y[2]+position_x[3]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[0]+position_x[2]] = board2[position_y[1]+position_x[2]] = board2[position_y[1]+position_x[3]] = board2[position_y[2]+position_x[3]] = turnLeft+3;
                        return 1;}
                         break;
                case 19:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[1]+position_x[2]] && board[position_y[1]+position_x[3]] && board[position_y[2]+position_x[3]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[1]+position_x[2]] = board[position_y[1]+position_x[3]] = board[position_y[2]+position_x[3]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[1]+position_x[2]] = board2[position_y[1]+position_x[3]] = board2[position_y[2]+position_x[3]] = turnLeft+3;
                        return 1;}
                         break;
                case 20:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[1]+position_x[2]] && board[position_y[1]+position_x[3]] && board[position_y[2]+position_x[2]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[1]+position_x[2]] = board[position_y[1]+position_x[3]] = board[position_y[2]+position_x[2]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[1]+position_x[2]] = board2[position_y[1]+position_x[3]] = board2[position_y[2]+position_x[2]] = turnLeft+3;
                        return 1;}
                         break;
                case 21:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[1]+position_x[0]] && board[position_y[1]+position_x[2]] && board[position_y[2]+position_x[1]]){
                        board[position_y[0]+position_x[1]] = board[position_y[1]+position_x[1]] = board[position_y[1]+position_x[0]] = board[position_y[1]+position_x[2]] = board[position_y[2]+position_x[1]] = 0;
                        board2[position_y[0]+position_x[1]] = board2[position_y[1]+position_x[1]] = board2[position_y[1]+position_x[0]] = board2[position_y[1]+position_x[2]] = board2[position_y[2]+position_x[1]] = turnLeft+3;
                        return 1;}
                         break;
                default: return 0;
                }
                return 0;
        }
        bool verifier2(int figure_num,int x,int y){
            y+=7;x+=7;
            int position_y[5]={14*y,14*(y-1),14*(y-2),14*(y-3),14*(y-4)},position_x[4]={x-1,x,x+1,x+2};
                switch(ROTATION_STATE){
                    case 1:
                            for(int i=1;i<5;i++){
                                position_y[i]+=(i)*14+i;
                            }
                            position_x[0]-=13;
                            position_x[2]+=13;
                            position_x[3]+=26;
                        break;
                    case 2:
                        for(int i=1;i<5;i++){
                            position_y[i]+=(i)*28;
                        }
                        position_x[0]+=2;
                        position_x[2]-=2;
                        position_x[3]-=3;
                        break;
                    case 3:
                        for(int i=1;i<5;i++){
                            position_y[i]+=(i)*14-i;
                        }
                        position_x[0]+=15;
                        position_x[2]-=15;
                        position_x[3]-=30;
                        break;
                }
                switch(figure_num){
                //implement movement limits in graphics !!!
                case 1:if(board[position_y[0]+position_x[1]]) {
                        return 1;}
                        break;
                case 2:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]]){
                     return 1;}
                    break;
                case 3:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]]){
                            return 1;}
                            break;
                case 4:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[1]+position_x[2]]){
                        return 1;}
                        break;
                case 5:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[3]+position_x[1]]){
                        return 1;}
                        break;
                case 6:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[0]]){
                        return 1;}
                        break;
                case 7:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[1]+position_x[2]]){
                        return 1;}
                        break;
                case 8:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[1]+position_x[2]] && board[position_y[0]+position_x[2]]){
                        return 1;}
                        break;
                case 9:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[2]] && board[position_y[0]+position_x[2]] && board[position_y[1]+position_x[3]]){
                        return 1;}
                         break;
                case 10:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[3]+position_x[1]] && board[position_y[4]+position_x[1]]){
                        return 1;}
                        break;
                case 11:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[3]+position_x[1]] && board[position_y[3]+position_x[0]]){
                        return 1;}
                         break;
                case 12:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[0]] && board[position_y[3]+position_x[0]]){
                        return 1;}
                         break;
                case 13:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[0]] && board[position_y[1]+position_x[0]] ){
                        return 1;}
                         break;
                case 14:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[0]] && board[position_y[0]+position_x[0]]){
                        return 1;}
                         break;
                case 15:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[3]+position_x[1]] && board[position_y[1]+position_x[2]]){
                        return 1;}
                         break;
                case 16:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[0]] && board[position_y[2]+position_x[2]]){
                        return 1;}
                         break;
                case 17:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[2]+position_x[1]] && board[position_y[2]+position_x[2]] && board[position_y[2]+position_x[3]]){
                        return 1;}
                         break;
                case 18:if(board[position_y[0]+position_x[1]] && board[position_y[0]+position_x[2]] && board[position_y[1]+position_x[2]] && board[position_y[1]+position_x[3]] && board[position_y[2]+position_x[3]]){
                        return 1;}
                         break;
                case 19:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[1]+position_x[2]] && board[position_y[1]+position_x[3]] && board[position_y[2]+position_x[3]]){
                        return 1;}
                         break;
                case 20:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[1]+position_x[2]] && board[position_y[1]+position_x[3]] && board[position_y[2]+position_x[2]]){
                        return 1;}
                         break;
                case 21:if(board[position_y[0]+position_x[1]] && board[position_y[1]+position_x[1]] && board[position_y[1]+position_x[0]] && board[position_y[1]+position_x[2]] && board[position_y[2]+position_x[1]]){
                        return 1;}
                         break;
                default: return 0;
                }
            return 0;
        }
        bool locate(int figure_num,int init_pos_x,int init_pos_y){
            //verifier
            if(verifier(figure_num,init_pos_x,init_pos_y)){
                if(turnLeft){
                    left_player[figure_num-1]=0;
                    board_figures_x[figure_num-1]=init_pos_x;
                    board_figures_y[figure_num-1]=init_pos_y;
                    R_STATES[figure_num-1]=ROTATION_STATE;
                    //first available selector
                    for(int i=0;i<21;i++){
                        if(left_player[i]){
                            setSelected(0,i+1);
                            break;
                        }
                    }
                    return 1;
                }
                else{
                    //21 below means reference to the right player figure
                    right_player[figure_num-1]=0;
                    board_figures_x[21+figure_num-1]=init_pos_x;
                    board_figures_y[21+figure_num-1]=init_pos_y;
                    R_STATES[21+figure_num-1]=ROTATION_STATE;
                    for(int i=0;i<21;i++){
                        if(right_player[i]){
                            setSelected(1,i+1);
                            break;
                        }
                    }
                    return 1;
                }
            }
            return 0;
        }
    //closes available selected left/right/up/down
    //0-left 1-right 2-top 3-bottom
    int closestAvailable(int direction,int left){
        if(left){
            switch(direction){
            case 0:
                for(int i=selected[0]-2+(selected[0]-2<0?22:0);i>=0;i--){
                    if(left_player[i]){
                        return i+1;
                    }
                    if((!left)&&(i==selected[0]-1)){
                        return 0;
                    }
                    if(i==0){
                        i=20;
                        left=0;
                    }

                }
                break;
            case 1:
                for(int i=selected[0]+(selected[0]>20?-21:0);i<21;i++){
                    if(left_player[i]){
                        return i+1;
                    }
                    if(!(left)&&(i==selected[0]-1)){
                            return 0;
                    }
                    if(i==20){
                        i=0;
                        left=0;
                    }
                }
                break;
            case 2:
                for(int i=(selected[0]-4<0?20:0)+selected[0]-4;i>=0;i--){
                        if(left_player[i]){
                            return i+1;
                        }
                        if(!(left)&&(i==selected[0]-1)){
                            return 0;
                        }
                        if(i==0){
                            i=20;
                            left=0;
                        }
                }
                break;
            case 3:
                 for(int i=(selected[0]+2>20?-20:0)+selected[0]+2;i>=0;i++){
                        if(left_player[i]){
                            return i+1;
                        }
                        if(!(left)&&(i==selected[0]-1)){
                            return 0;
                        }
                        if(i==20){
                            i=0;
                            left=0;
                        }
                }
                break;
            }
        }
        else{

            switch(direction){
            case 0:
                for(int i=selected[1]-2+(selected[1]-2<0?22:0);i>=0;i--){
                    if(right_player[i]){
                        return i+1;
                    }
                    if(i==0){
                        i=20;
                        left=1;
                    }
                    if(left&&(i==selected[1]-1)){
                        return 0;
                    }
                }
                break;
            case 1:
                for(int i=selected[1]+(selected[1]>20?-21:0);i<21;i++){
                    if(right_player[i]){
                        return i+1;
                    }
                    if(left&&(i==selected[1]-1)){
                        return 0;
                    }
                    if(i==20){
                        i=0;
                        left=1;
                    }
                }
                break;
            case 2:
                for(int i=(selected[1]-4<0?20:0)+selected[1]-4;i>=0;i--){
                        if(right_player[i]){
                            return i+1;
                        }
                        if(left&&(i==selected[1]-1)){
                            return 0;
                        }
                        if(i==0){
                            i=20;
                            left=1;
                        }
                }
                break;
            case 3:
                 for(int i=(selected[1]+2>20?-20:0)+selected[1]+2;i>=0;i++){
                        if(right_player[i]){
                            return i+1;
                        }
                        if(left&&(i==selected[1]-1)){
                            return 0;
                        }
                        if(i==20){
                            i=0;
                            left=1;
                        }
                }
                break;
            }
        }
    }
    int score(bool left){
        int total=89,i=0;
        if(left){
            for(i=0;i<20;i++){
                if(left_player[i]==1){
                    total-=totalCubes(i+1);
                }
            }
            if(total==89) total +=15;
        }else{
            for(i=0;i<20;i++){
                if(right_player[i]==1){
                    total-=totalCubes(i+1);
                }
                if(total==89) total +=15;
            }
        }
        return 0;
    }
};
gameBox GB;

//basic units
void light(float amb[]){
    glLightfv(GL_LIGHT0,GL_DIFFUSE,amb);
}
void Cube(){
    //top
    glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glVertex3f(0.2,0.2,0.2);
        glVertex3f(0.2,0.2,-0.2);
        glVertex3f(-0.2,0.2,-0.2);
        glVertex3f(-0.2,0.2,0.2);
    glEnd();
    //bottom
    glBegin(GL_QUADS);
        glNormal3f(0,-1,0);
        glVertex3f(0.2,-0.2,0.2);
        glVertex3f(0.2,-0.2,-0.2);
        glVertex3f(-0.2,-0.2,-0.2);
        glVertex3f(-0.2,-0.2,0.2);
    glEnd();
    //left
    glBegin(GL_QUADS);
        glNormal3f(0,0,-1);
        glVertex3f(0.2,0.2,-0.2);
        glVertex3f(0.2,-0.2,-0.2);
        glVertex3f(-0.2,-0.2,-0.2);
        glVertex3f(-0.2,0.2,-0.2);
    glEnd();
    //right
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glVertex3f(0.2,0.2,0.2);
        glVertex3f(0.2,-0.2,0.2);
        glVertex3f(-0.2,-0.2,0.2);
        glVertex3f(-0.2,0.2,0.2);
    glEnd();
    //up
    glBegin(GL_QUADS);
        glNormal3f(-1,0,0);
        glVertex3f(-0.2,0.2,0.2);
        glVertex3f(-0.2,-0.2,0.2);
        glVertex3f(-0.2,-0.2,-0.2);
        glVertex3f(-0.2,0.2,-0.2);
    glEnd();
    //down
    glBegin(GL_QUADS);
        glNormal3f(1,0,0);
        glVertex3f(0.2,0.2,0.2);
        glVertex3f(0.2,-0.2,0.2);
        glVertex3f(0.2,-0.2,-0.2);
        glVertex3f(0.2,0.2,-0.2);
    glEnd();

}
//figures
void figure(float mycolor[],int rotator,int number){

    light(mycolor);
    glPushMatrix();
        glScalef(0.572814,0.572814,0.5);
        Cube();
        switch(number){
            case 2:
                glTranslatef(0,-1*(0.05+float(2.85/7.0)),0);
                Cube();break;
            case 3:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();break;
            case 4:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.05+float(3/7.0)),0,0);
                Cube();break;
            case 5:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();break;
            case 6:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(-1*(0.07+float(3/7.0)),0,0);
                Cube();break;
            case 7:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),(0.05+float(3/7.0)),0);
                Cube();break;
            case 8:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef(0,(0.05+float(3/7.0)),0);
                Cube();break;
            case 9:
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();break;
            case 10:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();break;
            case 11:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(-1*(0.07+float(3/7.0)),0,0);
                Cube();break;
            case 12:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(-1*(0.05+float(3/7.0)),0,0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();break;
            case 13:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(-1*(0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef(0,(0.05+float(3/7.0)),0);
                Cube();break;
            case 14:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(-1*(0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef(0,2*(0.05+float(3/7.0)),0);
                Cube();break;
            case 15:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),2*(0.05+float(3/7.0)),0);
                Cube();break;
            case 16:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef(-2*(0.07+float(3/7.0)),0,0);
                Cube();break;
            case 17:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();break;
            case 18:
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();break;
            case 19:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();break;
            case 20:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),0,0);
                Cube();
                glTranslatef(-1*(0.07+float(3/7.0)),-1*(0.05+float(3/7.0)),0);
                Cube();break;
            case 21:
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(0,-1*(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef((0.07+float(3/7.0)),(0.05+float(3/7.0)),0);
                Cube();
                glTranslatef(-2*(0.07+float(3/7.0)),0,0);
                Cube();break;
            default:break;
        }
    glPopMatrix();
}
//boards
void markers(){

    float frame_color[]={0,0.1,0.1,0.2};
    float i=1;
    for(i=-7;i<8;i++){
        glPushMatrix();
            light(frame_color);
            glColor3f(0.8,0.8,0);
            glTranslatef(0,float(i/3.5),0);
            glScalef(10,0.05,0.3);
            Cube();
        glPopMatrix();
    }
    i=1;
    for(i=-7;i<8;i++){
        glPushMatrix();
            light(frame_color);
            glColor3f(0.8,0.8,0);
            glTranslatef(float(i/3.5),0,0);
            glScalef(0.05,10,0.3);
            Cube();
        glPopMatrix();
    }
}
void board(){
    glPushMatrix();
        float boardColor[]={0.2,0.2,0.2,1};
        light(boardColor);
        //glRotatef(angle,0,0,1);
        glScalef(10,10,0.25);
        Cube();

    glPopMatrix();
}
void pickBoard(float pos,float color[],bool black,int selected,bool Left,bool anime){
    glDisable(GL_LIGHT0);
    light(color);

    glPushMatrix();
        glEnable(GL_LIGHT0);
        glTranslatef(pos,0,0);
        glPushMatrix();
        glScalef(3,10,0.2);
        Cube();

        glPopMatrix();
        //figure lister
        float figure_color[]={0.5,0.5,0.5,0.5};
        if(black){
            figure_color[0]=0.5;
            figure_color[1]=0.5;
            figure_color[2]=0.5;
        }
        glTranslatef(-0.85+(BU_X/4.0),float(14/7)-(BU_Y/2),0.02);
        //figure(figure_color,0,2);
        for(int y=0;y<7;y++){
            glPushMatrix();
            glScalef(0.333,0.333,1);
            for(int x=0;x<3;x++){
                glTranslatef(1,0,0.0);
                if(Left){
                    if(!GB.getLeftAvail(x+y*3)){
                        continue;
                    }
                }
                else{
                    if(!GB.getRightAvail(x+y*3)){
                        continue;
                    }
                }
                if(x*y==12)glTranslatef(0.2,0,0);
                glPushMatrix();
                //selected animation
                if(anime && (selected == ((x+1)+(y*3)))){
                    glRotatef(selectedAngleCurrent,0,0,1);
                    if(selectedDirection==1){
                            if(selectedAngleCurrent>15.0){
                                selectedDirection=-1;
                            }
                    }else{
                        if(selectedAngleCurrent<-15.0){
                            selectedDirection=1;
                        }
                    }
                    selectedAngleCurrent=selectedAngleCurrent+selectedDirection;
                }
                figure(figure_color,0,(x+1)+(y*3));
                glPopMatrix();
            }
            glPopMatrix();
     //       figure(figure_color,0,i);
            glTranslatef(0,-0.571428,0);
        }
        glDisable(GL_LIGHT0);
    glPopMatrix();
    glEnable(GL_LIGHT0);

}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //*cos(toRads(cameraAngle))
    //*sin(toRads(cameraAngle))
    gluLookAt(cameraX,cameraY,cameraZ,0,0,0,0,1,0);
    GLfloat lamp_position[] = {0.0,2.0,2.0,0.0},spot[]={0,0,0};
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,spot);
    glLightfv(GL_LIGHT0,GL_POSITION,lamp_position);

//    glRotatef(angle2,0,0,0);
    //glTranslatef(0,0,-5);
    board();
//    glColor3f(1,0,0);
    markers();
    float pickBoard_color_a[]={200/256.0,200/256.0,0/256.0,1};
    float pickBoard_color_b[]={86/256.0,0/256.0,170/256.0,1};
    pickBoard(2.7,pickBoard_color_a,1,GB.getSelected(1),0,!(turnLeft));
    pickBoard(-2.7,pickBoard_color_b,0,GB.getSelected(0),1,turnLeft);
    //glColor3f(1,0,0);
    for(int i=0;i<21;i++){
        if(!GB.getLeftAvail(i)){
            glPushMatrix();
                glTranslatef(initial_position+float(board_figures_x[i]/3.53),initial_position+float(board_figures_y[i]/3.53),0.05);
                glRotatef(90*R_STATES[i],0,0,1);
                figure(pickBoard_color_b,0,i+1);
            glPopMatrix();
        }
        if(!GB.getRightAvail(i)){
            glPushMatrix();
                glTranslatef(initial_position+float(board_figures_x[i+21]/3.53),initial_position+float(board_figures_y[i+21]/3.53),0.05);
                glRotatef(90*R_STATES[i+21],0,0,1);
                figure(pickBoard_color_a,0,i+1);
            glPopMatrix();
        }
    }
    float unavail_color[]={1.0,0.0,0.0,0.5};
    //locating object
    if(locateMode){
        glTranslatef(initial_position+float(X_LOCATE/3.53),initial_position+float(Y_LOCATE/3.53),0.1);
                glRotatef(90*ROTATION_STATE,0,0,1);
        if(turnLeft){
            if(GB.verifier2(GB.getSelected(0),X_LOCATE,Y_LOCATE)){

                figure(pickBoard_color_b,1,GB.getSelected(0));}
            else{
                figure(unavail_color,1,GB.getSelected(0));}
        }else{
            if(GB.verifier2(GB.getSelected(1),X_LOCATE,Y_LOCATE)){
                figure(pickBoard_color_a,1,GB.getSelected(1));}
            else{
                figure(unavail_color,1,GB.getSelected(1));
            }
        }
    }
}

float setCameraX=cameraX,setCameraY=cameraY,setCameraZ=cameraZ;
int gameNotOver=1;
void updateMargins(int selectedFigure,int rotateCase){
    switch(rotateCase){
        case 0:
            MARGINS[0]=cubesInFigureDown(selectedFigure);
            MARGINS[2]=0;
            MARGINS[1]=cubesInFigureLeft(selectedFigure);
            MARGINS[3]=cubesInFigureRight(selectedFigure);
            break;
        case 1:
            MARGINS[0]=cubesInFigureLeft(selectedFigure);
            MARGINS[2]=cubesInFigureRight(selectedFigure);
            MARGINS[1]=0;
            MARGINS[3]=cubesInFigureDown(selectedFigure);
            break;
        case 2:
            MARGINS[0]=0;
            MARGINS[2]=cubesInFigureDown(selectedFigure);
            MARGINS[1]=cubesInFigureRight(selectedFigure);
            MARGINS[3]=cubesInFigureLeft(selectedFigure);
            break;
        case 3:
            MARGINS[0]=cubesInFigureRight(selectedFigure);
            MARGINS[2]=cubesInFigureLeft(selectedFigure);
            MARGINS[1]=cubesInFigureDown(selectedFigure);
            MARGINS[3]=0;
            break;
        }
}
void rotationHandler(int figure_num){
    updateMargins(figure_num,((ROTATION_STATE+1)%4));
    if(Y_LOCATE-MARGINS[0]<-7){
        Y_LOCATE+=MARGINS[0]-Y_LOCATE-7;
    }
    if(Y_LOCATE+MARGINS[2]>6){
        Y_LOCATE-=Y_LOCATE+MARGINS[2]-6;
    }
    if(X_LOCATE-MARGINS[1]<-6){
        X_LOCATE+=MARGINS[1]-X_LOCATE-7;
    }
    if(X_LOCATE+MARGINS[3]>5){
        X_LOCATE-=X_LOCATE+MARGINS[3]-5;
    }
}
void keyInputManagement(){

//        if(keyPressed_U)angle2+=0.01;
//        if(keyPressed_D) angle2-=0.01;
//        if(keyPressed_L)angle+=0.01;
//        if(keyPressed_R)angle-=0.01;
    SDL_Event myevent;
    if(SDL_PollEvent(&myevent)) {

        if(myevent.type==SDL_KEYDOWN) {
            //if the mode is locate
            if(locateMode){
                //calculating margins for the selected figure
                updateMargins(GB.getSelected(!(turnLeft)),ROTATION_STATE);
                //movement keys setup
                switch(myevent.key.keysym.sym) {
                    case SDLK_DOWN:  if(Y_LOCATE-MARGINS[0]-1>-8){Y_LOCATE-=1;}keyPressed_D=1;break;
                    case SDLK_UP:  if(Y_LOCATE+MARGINS[2]+1<7){Y_LOCATE+=1;}keyPressed_U=1;break;
                    case SDLK_LEFT:  if(X_LOCATE-MARGINS[1]-1>-8){X_LOCATE-=1;}keyPressed_L=1;break;
                    case SDLK_RIGHT: if(X_LOCATE+MARGINS[3]+1<7){X_LOCATE+=1;} keyPressed_R=1;break;
                    //perspective manager
                    case SDLK_1: setCameraY=0.0;setCameraX=0.0;break;
                    case SDLK_2: setCameraY=-2.0;setCameraX=0.0;break;
                    case SDLK_3: setCameraY=-2.0;setCameraX=-2.0;break;
                    case SDLK_4: setCameraY=-2.0;setCameraX=2.0;break;
                    case SDLK_5: setCameraY=2;setCameraX=-2;break;
                    case SDLK_6: setCameraY=2;setCameraX=2;break;
                    //mode management
                    case SDLK_ESCAPE: X_LOCATE=Y_LOCATE=0;locateMode=0; break;
                    case SDLK_TAB: rotationHandler(GB.getSelected(!(turnLeft)));ROTATION_STATE+=1;ROTATION_STATE%=4;break;

                    case SDLK_RETURN:
                        if(GB.locate(GB.getSelected(!(turnLeft)),X_LOCATE,Y_LOCATE)){
                            turnLeft=!(turnLeft);
                            X_LOCATE=ROTATION_STATE=Y_LOCATE=0;
                            locateMode=0;
                        }
                        break;
                    default:break;
                }
            }
            else{
                switch(myevent.key.keysym.sym) {
                    case SDLK_UP:
                        if(GB.getSelected(!(turnLeft))-3>0){
                            GB.setSelected(!(turnLeft),GB.closestAvailable(2,turnLeft));
                        }
                        keyPressed_U=1;break;
                    case SDLK_DOWN:if(GB.getSelected(!(turnLeft))+3<22){
                            GB.setSelected(!(turnLeft),GB.closestAvailable(3,turnLeft));
                        } keyPressed_D=1;break;
                    case SDLK_LEFT:  if(GB.getSelected(!(turnLeft))-1>0){
                            GB.setSelected(!(turnLeft),GB.closestAvailable(0,turnLeft));
                        }keyPressed_L=1;break;
                    case SDLK_RIGHT: if(GB.getSelected(!(turnLeft))+1<22){
                            GB.setSelected(!(turnLeft),GB.closestAvailable(1,turnLeft));
                        } keyPressed_R=1;break;
                    case SDLK_TAB: ROTATION_STATE+=1;ROTATION_STATE%=4;break;
                    //perspective manager
                    case SDLK_1: setCameraY=0.0;setCameraX=0.0;break;
                    case SDLK_2: setCameraY=-2.0;setCameraX=0.0;break;
                    case SDLK_3: setCameraY=-2.0;setCameraX=-2.0;break;
                    case SDLK_4: setCameraY=-2.0;setCameraX=2.0;break;
                    case SDLK_5: setCameraY=2;setCameraX=-2;break;
                    case SDLK_6: setCameraY=2;setCameraX=2;break;
                    //selection management
                    case SDLK_SPACE: turnLeft=!(turnLeft);break;
                    case SDLK_RETURN: locateMode=1;break;
                    case SDLK_ESCAPE: gameNotOver=0;break;
                    default: break;
                }
            }
        }
        if(myevent.type==SDL_KEYUP){
            switch(myevent.key.keysym.sym){
    //                    case SDLK_UP:  angle2+=0.142857;keyPressed_U=0; ;break;
    //                    case SDLK_DOWN:  angle2-=0.142857;keyPressed_D=0;break;
    //                   case SDLK_LEFT:  angle+=0.142857;keyPressed_L=0;break;
    //                   case SDLK_RIGHT: angle-=0.142857; keyPressed_R=0;break;
            }
            myDir=0;
        }
        if(myevent.type==SDL_QUIT){
            gameNotOver=0;
        }
    }
}
void cameraAnimationManagement(){

        if(cameraY!=setCameraY){
            if(cameraY<setCameraY){
                cameraY+=animation_Speed;
                if(setCameraY-cameraY<0.02){
                    cameraY=setCameraY;
                }
            }else{
                cameraY-=animation_Speed;
                if(cameraY-setCameraY<0.02){
                    cameraY=setCameraY;
                }
            }
        }
        if(cameraX!=setCameraX){
            if(cameraX<setCameraX){
                cameraX+=animation_Speed;
                if(setCameraX-cameraX<0.02){
                    cameraX=setCameraX;
                }
            }else{
                cameraX-=animation_Speed;
                if(cameraX-setCameraX<0.02){
                    cameraX=setCameraX;
                }
            }
        }
}

int main(int argc, char* args[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetVideoMode(1280,720,32,SDL_SWSURFACE|SDL_OPENGL);


    init();
    while (gameNotOver==1)
    {
        cameraAnimationManagement();
        keyInputManagement();
        display();
        SDL_GL_SwapBuffers();
    }

    SDL_Quit();
    return 0;
}


