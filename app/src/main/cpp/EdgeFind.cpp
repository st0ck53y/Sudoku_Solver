//
// Created by TomLaptop on 14/01/2017.
//

#include "EdgeFind.h"
#include <stdlib.h>

EdgeFind::EdgeFind(int imageWidth, int imageHeight) {
    w = imageWidth;
    h = imageHeight;
    gradient = (int*) malloc((w*h)*sizeof(int));
    direction = (int*) malloc((w*h)*sizeof(int));
}

EdgeFind::~EdgeFind() {
    free(gradient);
    free(direction);
}

void EdgeFind::computeGradientAngles(int* imgIn, int* preCompDir) {
    for (int y = 0; y < h - 1; y++) {
        int yOffs = y * w;
        int yPos  = yOffs + w;
        for (int x = 0; x < w - 1; x++) {
            int xd = computeXDerivative(imgIn[yOffs + x], imgIn[yOffs + x + 1], imgIn[yPos + x], imgIn[yPos + x + 1]);
            int yd = computeYDerivative(imgIn[yOffs + x], imgIn[yOffs + x + 1], imgIn[yPos + x], imgIn[yPos + x + 1]);
            gradient[yOffs+x] = (int) round(sqrt((xd*xd)+(yd*yd)));
            int yIdx = (yd & 0xff) | (yd<0?1<<8:0);
            int xIdx = (xd & 0xff) | (xd<0?1<<8:0);
            direction[yOffs+x] = preCompDir[yIdx<<9|(xIdx&0x1ff)];
        }
    }
}

void EdgeFind::suppressNonMaxima(int* imgIn) {
    for (int y = 0; y < h - 1; y++) {
        int yOffs = y * w;
        for (int x = 0; x < w -1; x++) {
            if (y==0 || x==0) {
                imgIn[yOffs+x] = 0;
                continue;
            }
            int dir = direction[yOffs+x];
            if (dir < -67 || dir > 67) { //Horizontal
                if (gradient[yOffs+x] < gradient[yOffs+x+w] || gradient[yOffs+x] < gradient[yOffs+x-w]) {
                    imgIn[yOffs+x] = 0;
                } else {
                    imgIn[yOffs+x] = gradient[yOffs+x];
                }
            } else if (dir > -23 && dir < 23) { //Vertical
                if (gradient[yOffs+x] < gradient[yOffs+x+1] || gradient[yOffs+x] < gradient[yOffs+x-1]) {
                    imgIn[yOffs+x] = 0;
                } else {
                    imgIn[yOffs+x] = gradient[yOffs+x];
                }
            } else if (dir < -22) { //down right - up left
                if (gradient[yOffs+x] < gradient[yOffs+x+(w-1)] || gradient[yOffs+x] < gradient[yOffs+x-(w-1)]) {
                    imgIn[yOffs+x] = 0;
                } else {
                    imgIn[yOffs+x] = gradient[yOffs+x];
                }
            } else { //up right - down left
                if (gradient[yOffs+x] < gradient[yOffs+x+(w+1)] || gradient[yOffs+x] < gradient[yOffs+x-(w+1)]) {
                    imgIn[yOffs+x] = 0;
                } else {
                    imgIn[yOffs+x] = gradient[yOffs+x];
                }
            }
        }
    }
}

void** EdgeFind::applyThreshold(int* imgIn, int tL, int tH) {
    int a = w * h;
    int* lower = (int*) malloc(a*sizeof(int));
    int* higher = (int*) malloc(a* sizeof(int));
    bool* visited = (bool*) malloc(a* sizeof(bool));
    for (int i = 0; i < a; i++) {
        if (imgIn[i] > tL) {
            lower[i] = imgIn[i];
            if (imgIn[i] > tH) { //this way ensures its in lower and higher a little nicer
                higher[i] = imgIn[i];
            }
        }
    }
    int curEdgeMax = 512;
    void** edges = (void**) malloc(curEdgeMax*sizeof(int));
    int edgeNum = 0;

    //hysteresis
    for (int y = 1; y < h - 1; y++) {
        int yOffs = y * w;
        for (int x = 1; x < w - 1; x++) {
            if (visited[yOffs+x]) continue; //skip pixel if already done
            if (higher[yOffs+x] > 0) {
                if (edgeNum == curEdgeMax) {
                    curEdgeMax*=2;
                    edges = (void**)realloc(edges,(size_t)(curEdgeMax*sizeof(int)));
                }
                edges[edgeNum++] = traverseLine(visited,lower,y,x);
            }
        }
    }
    free(lower);
    free(higher);
    free(visited);
    edges = (void**)realloc(edges, (size_t)(edgeNum+1)*sizeof(int));
    edges[edgeNum] = 0;//null terminate
    return edges;
}

int EdgeFind::computeXDerivative(int a, int b, int c, int d) {
    return ((b - a) + (d - c))/2;
}

int EdgeFind::computeYDerivative(int a, int b, int c, int d) {
    return ((c - a) + (d - b))/2;
}

int* EdgeFind::calcThresholds(int dL, int dH) {
    int* t = (int*)malloc(2*sizeof(int));
//    t[0] = dL;
//    t[1] = dH;
//    return t;
    int tot = 0;
    int cnt = 0;
    for (int i = 0; i < (w*h); i++) {
        tot += gradient[i];
        if (gradient[i] > 0) cnt++;
    }
    if (cnt == 0) {
        t[0] = dL;
        t[1] = dH;
        return t;
    }
    int avg = tot / cnt;
    int totL = 0;
    int lCnt = 0;
    int totH = 0;
    int hCnt = 0;
    for (int i = 0; i < w*h; i++) {
        if (gradient[i] < avg) {
            totL += gradient[i];
            if (gradient[i] > 0) lCnt++;
        } else {
            totH += gradient[i];
            hCnt++;
        }
    }
    if (lCnt == 0) {
        t[0] = dL;
    } else {
        t[0] = (2*totL/lCnt);
    }
    if (hCnt == 0) {
        t[1] = dH;
    } else{
        t[1] = (2*totH/hCnt);
    }
    return t;
}

void** EdgeFind::cullShortEdges(void** edges, int thresh) {
    int edc = 0;
    int longer = 0;
    int curLenMax = 128;
    void** longEdges = (void**)malloc(curLenMax*sizeof(int));
    while (edges[edc]!=0) {
        if (longer == curLenMax) {
            curLenMax*=2;
            longEdges = (void**)realloc(longEdges, (size_t)(curLenMax*sizeof(int)));
        }
        Line* root = (Line*) edges[edc];
        Line* trav = root;
        bool lng = true;
        //traverse edges until reaching depth of threshold, then add to longEdges
        for (int i = 0; i <= thresh; i++) {
            if (trav->next == 0) {
                lng = false;
                break;
            }
            trav = trav->next;
        }
        if (lng) {
            longEdges[longer++] = edges[edc];
        }
        edc++;
    }
    longEdges = (void**)realloc(longEdges, (size_t)((longer+1)*sizeof(int)));
    longEdges[longer] = 0; //null terminate the array
    return longEdges;
}

//returns a line, modifies the visited array
void* EdgeFind::traverseLine(bool* visited, int* lower, int y, int x) {
    int imgLen = w * h;
    Line* root = new Line;
    root->p = (y * w) + x;
    Line* cur = root;

    bool chk;
    int cy,cx,cn;
    cy=y;
    cx=x;
    do {
        chk = false;
        visited[(cy*w)+cx]=true;
        cn = ((cy-1)*w)+(cx-1);
        if (walked(visited,lower,imgLen,cn,cy,cx,-1,-1)) {
            appendToLine(cur,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,-1,0)) {
            appendToLine(cur,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,-1,1)) {
            appendToLine(cur,cn);
            chk = true;
            continue;
        }
        cn = (cy*w)+(cx-1);
        if (walked(visited,lower,imgLen,cn,cy,cx,0,-1)) {
            appendToLine(cur,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,0,0)) {
            appendToLine(cur,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,0,1)) {
            appendToLine(cur,cn);
            chk = true;
            continue;
        }
        cn = ((cy+1)*w)+(cx-1);
        if (walked(visited,lower,imgLen,cn,cy,cx,1,-1)) {
            appendToLine(cur,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,1,0)) {
            appendToLine(cur,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,1,1)) {
            appendToLine(cur,cn);
            chk = true;
        }
    } while(chk);
    cur->next = 0;
    cy=y;
    cx=x;
    do {
        chk = false;
        visited[(cy*w)+cx]=true;
        cn = ((cy-1)*w)+(cx-1);
        if (walked(visited,lower,imgLen,cn,cy,cx,-1,-1)) {
            prependToLine(root,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,-1,0)) {
            prependToLine(root,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,-1,1)) {
            prependToLine(root,cn);
            chk = true;
            continue;
        }
        cn = (cy*w)+(cx-1);
        if (walked(visited,lower,imgLen,cn,cy,cx,0,-1)) {
            prependToLine(root,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,0,0)) {
            prependToLine(root,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,0,1)) {
            prependToLine(root,cn);
            chk = true;
            continue;
        }
        cn = ((cy+1)*w)+(cx-1);
        if (walked(visited,lower,imgLen,cn,cy,cx,1,-1)) {
            prependToLine(root,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,1,0)) {
            prependToLine(root,cn);
            chk = true;
            continue;
        }
        cn++;
        if (walked(visited,lower,imgLen,cn,cy,cx,1,1)) {
            prependToLine(root,cn);
            chk = true;
        }
    } while(chk);
    return (void*) root;
}

bool EdgeFind::walked(bool* visited, int* lower, int imgLen, int cn, int& cy, int& cx, int yc, int xc) {
    if (cn < imgLen && cn >= 0 && !visited[cn]){
        if (lower[cn] > 0) {
            cy+=yc;
            cx+=xc;
            return true;
        }
    }
    return false;
}

void EdgeFind::appendToLine(Line*& cur, int val) {
    cur->next = new Line;
    cur = cur->next;
    cur->p = val;
}

void EdgeFind::prependToLine(Line*& root, int val) {
    Line* nRoot = new Line;
    nRoot->p = val;
    nRoot->next = root;
    root = nRoot;
}

void EdgeFind::paintEdges(int* img, void** edges) {
    for (int i = 0; i < w * h; i++) {
        img[i] = 0;
    }
    int edc = 0;
    while (edges[edc]!=0) {
        Line* edge = (Line*) edges[edc++];
        while (edge->next != 0) {
            img[edge->p] = 255;
            edge = edge->next;
        }
    }
}