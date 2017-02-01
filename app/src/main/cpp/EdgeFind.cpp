//
// Created by TomLaptop on 14/01/2017.
//

#include "EdgeFind.h"
#include <stdlib.h>

EdgeFind::EdgeFind(int imageWidth, int imageHeight) {
    w = imageWidth;
    h = imageHeight;
    m_ancMax = 256;
    m_edgeMax = 256;
    m_edges = (void**) malloc(m_edgeMax*sizeof(int));
    m_gradient = (int*) malloc((w*h)*sizeof(int));
    m_direction = (int*) malloc((w*h)*sizeof(int));
    m_simpDirs = (bool*) malloc((w*h)*sizeof(bool));
    m_anchors = (int*) malloc(m_ancMax*sizeof(int));
    m_visited = (bool*) calloc((size_t)(w*h),sizeof(bool));
}

EdgeFind::~EdgeFind() {
    free(m_edges);
    free(m_gradient);
    free(m_direction);
    free(m_simpDirs);
    free(m_anchors);

    free(m_visited);
}

void EdgeFind::computeGradientAngles(int* imgIn, int* preCompDir) {
    for (int y = 0; y < h - 1; y++) {
        int yOffs = y * w;
        int yPos  = yOffs + w;
        for (int x = 0; x < w - 1; x++) {
            int xd = computeXDerivative(imgIn[yOffs + x], imgIn[yOffs + x + 1], imgIn[yPos + x], imgIn[yPos + x + 1]);
            int yd = computeYDerivative(imgIn[yOffs + x], imgIn[yOffs + x + 1], imgIn[yPos + x], imgIn[yPos + x + 1]);
            m_gradient[yOffs+x] = (int) round(sqrt((xd*xd)+(yd*yd)));
            int yIdx = (yd & 0xff) | (yd<0?1<<8:0);
            int xIdx = (xd & 0xff) | (xd<0?1<<8:0);
            m_direction[yOffs+x] = preCompDir[yIdx<<9|(xIdx&0x1ff)];
        }
    }
}

void EdgeFind::simplifyDirections() {
    for (int i = 0; i < w*h; i++) {
        m_simpDirs[i] = !(m_direction[i] < 45 || m_direction[i] > 135);
    }
}

void EdgeFind::normalizeGradients() {
    int* cnt = (int*)calloc(256,sizeof(int));
    for (int i = 0; i < w*h; i++) {
        cnt[m_gradient[i]]++;
    }
    for (int i = 1; i < 256; i++) {
        cnt[i]=cnt[i]+cnt[i-1];
    }
    cnt[0] = 0; //0 stays as 0
    double var = 255.0 / ((double)w*h);
    for (int i = 0; i < w*h; i++) {
        m_gradient[i] = (int)(cnt[m_gradient[i]]*var);
    }
    free(cnt);
}

void EdgeFind::thresholdGradients() {
    u_int64_t t = 0;
    int cnt = 0;
    for (int i = 0; i < w*h; i++) {
        if (m_gradient[i] != 0)
            t += m_gradient[i];
        else
            cnt++;
    }
    int thresh = (int) t / ((w*h)-cnt);
    thresholdGradients(thresh);
}

void EdgeFind::thresholdGradients(int thresh) {
    for (int i = 0; i < w*h; i++) {
        if (m_gradient[i] < thresh) {
            m_gradient[i] = 0;
        }
    }
}

void EdgeFind::findAnchors() {
    findAnchors(16,4);
}

void EdgeFind::findAnchors(int thresh, int interval) {
    int curAncMax = m_ancMax;
    int curAnc = 0;
    for (int y = 2; y < h-2; y+=interval) {
        int yOffs = y*w;
        for (int x = 2; x < w-2; x+=interval) {
            if (m_simpDirs[yOffs+x]) {
                //up down
                if (m_gradient[yOffs+x]-m_gradient[(yOffs-w)+x] > thresh
                    && m_gradient[yOffs+x]-m_gradient[(yOffs-(2*w))+x] > thresh
                    && m_gradient[yOffs+x]-m_gradient[(yOffs+(2*w))+x] > thresh
                    && m_gradient[yOffs+x]-m_gradient[(yOffs+w)+x] > thresh) {
                    m_anchors[curAnc++] = yOffs+x;
                }
            } else {
                //left right
                if (m_gradient[yOffs+x]-m_gradient[yOffs+(x-1)] > thresh
                    && m_gradient[yOffs+x]-m_gradient[yOffs+(x-2)] > thresh
                    && m_gradient[yOffs+x]-m_gradient[yOffs+(x+2)] > thresh
                    && m_gradient[yOffs+x]-m_gradient[yOffs+(x+1)] > thresh) {
                    m_anchors[curAnc++] = yOffs+x;
                }
            }
            if (curAnc == curAncMax) {
                curAncMax*=2;
                m_anchors = (int*) realloc(m_anchors, curAncMax*sizeof(int));
            }
        }
    }
    m_anchors = (int*) realloc(m_anchors, (curAnc+1)*sizeof(int));
    m_anchors[curAnc] = 0; //null terminate
}

void EdgeFind::joinAnchors() {
    int edgeMax = m_edgeMax;
    int curEdge = 0;

    bool left;
    bool up;
    int cAnc = 0;
    while(m_anchors[cAnc]!= 0) {
        int pos = m_anchors[cAnc];
        if (!m_visited[pos]) {
            left = true;
            up = false;
            Line* dl = new Line();
            walkLine(pos,left,up,dl);
            left = false;
            up = true;
            m_visited[pos]=false;
            Line* ur = new Line();
            walkLine(pos,left,up,ur);
            mergeLines(dl,ur);
            m_edges[curEdge++] = (void*)dl;
        }
        cAnc++;
        if (curEdge == edgeMax) {
            edgeMax*=2;
            m_edges = (void**) realloc(m_edges,(size_t)(edgeMax*sizeof(int)));
        }
    }
    m_edges = (void**) realloc(m_edges,(size_t)((curEdge+1)*sizeof(int)));
    m_edges[curEdge] = 0;
}

void EdgeFind::mergeLines(Line *&ro, Line *rt) {
    if (rt == 0) return;
    if (ro == 0) {
        ro = rt;
        return;
    }
    if (ro->p != rt->p) {
        //TODO not sure what should happen here?
    }
    while(rt->next != 0) {
        prependToLine(ro, rt->p);
        rt = rt->next;
    }
}

void EdgeFind::walkLine(int pos, bool left, bool up, Line* line) {
    while(pos!=0) {
        int x = pos % w;
        int y = pos / w;
        if (m_simpDirs[pos]){
            if (up) {
                pos = goUp(x,y,left,line);
            } else {
                pos = goDown(x,y,left,line);
            }
        } else {
            if (left) {
                pos = goLeft(x,y,up,line);
            } else {
                pos = goRight(x,y,up,line);
            }
        }
    }
}

int EdgeFind::goLeft(int x, int y, bool& up, Line* line) {
    int ly = y;
    int pos = y*w + x;
    if (m_visited[pos]) return 0;
    while(m_gradient[pos]>0&&!m_simpDirs[pos]&&!m_visited[pos]) {
        ly = y;
        pos = (y*w) + x;
        m_visited[pos] = true;
        line->p = pos;
        line->next = new Line();
        line = line->next;
        if (m_gradient[(pos-w)-1]>m_gradient[pos-1]
            && m_gradient[pos-1]>m_gradient[(pos+w)-1]){
            y--;
            x--;
        } else if (m_gradient[(pos+w)-1]>m_gradient[pos-1]
                   && m_gradient[pos-1]>m_gradient[(pos-w)-1]) {
            y++;
            x--;
        } else {
            x--;
        }
    }
    up = y < ly;
    pos = (y*w) + x;
    if (m_gradient[pos]==0) return 0;
    else return pos;
}

int EdgeFind::goRight(int x, int y, bool& up, Line* line) {
    int ly = y;
    int pos = y*w + x;
    if (m_visited[pos]) return 0;
    while(m_gradient[pos]>0&&!m_simpDirs[pos]&&!m_visited[pos]) {
        ly = y;
        pos = (y*w) + x;
        m_visited[pos] = true;
        line->p = pos;
        line->next = new Line();
        line = line->next;
        if (m_gradient[(pos-w)+1]>m_gradient[pos+1]
            && m_gradient[pos+1]>m_gradient[(pos+w)+1]){
            y--;
            x++;
        } else if (m_gradient[(pos+w)+1]>m_gradient[pos+1]
                   && m_gradient[pos+1]>m_gradient[(pos-w)+1]) {
            y++;
            x++;
        } else {
            x++;
        }
    }
    up = y < ly;
    pos = (y*w) + x;
    if (m_gradient[pos]==0) return 0;
    else return pos;
}

int EdgeFind::goUp(int x, int y, bool& left, Line* line) {
    int lx = x;
    int pos = (y*w)+x;
    if (m_visited[pos]) return 0;
    while(m_gradient[pos]>0&&m_simpDirs[pos]&&!m_visited[pos]){
        lx = x;
        pos = (y*w) + x;
        m_visited[pos] = true;
        line->p = pos;
        line->next = new Line();
        line = line->next;
        if (m_gradient[(pos-w)-1]>m_gradient[(pos-w)]
            && m_gradient[(pos-w)]>m_gradient[(pos-w)+1]){
            y--;
            x--;
        } else if (m_gradient[(pos-w)+1]>m_gradient[(pos-w)]
                   && m_gradient[(pos-w)]>m_gradient[(pos-w)-1]){
            y--;
            x++;
        } else {
            y--;
        }
    }
    left = x < lx;
    pos = (y*w) + x;
    if (m_gradient[pos]==0) return 0;
    else return pos;
}

int EdgeFind::goDown(int x, int y, bool& left, Line* line) {
    int lx = x;
    int pos = (y*w)+x;
    if (m_visited[pos]) return 0;
    while(m_gradient[pos]>0&&m_simpDirs[pos]&&!m_visited[pos]){
        lx = x;
        pos = (y*w) + x;
        m_visited[pos] = true;
        line->p = pos;
        line->next = new Line();
        line = line->next;
        if (m_gradient[(pos+w)-1]>m_gradient[(pos+w)]
            && m_gradient[(pos+w)]>m_gradient[(pos+w)+1]){
            y++;
            x--;
        } else if (m_gradient[(pos+w)+1]>m_gradient[(pos+w)]
                   && m_gradient[(pos+w)]>m_gradient[(pos+w)-1]){
            y++;
            x++;
        } else {
            y++;
        }
    }
    left = x < lx;
    pos = (y*w) + x;
    if (m_gradient[pos]==0) return 0;
    else return pos;
}

int EdgeFind::computeXDerivative(int a, int b, int c, int d) {
    return ((b - a) + (d - c))/2;
}

int EdgeFind::computeYDerivative(int a, int b, int c, int d) {
    return ((c - a) + (d - b))/2;
}

void EdgeFind::cullShortEdges(int thresh) {
    m_edges = cullShortEdges(m_edges, thresh);
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

void EdgeFind::insertToLine(Line*& cur, int val) {
    Line* nxt = cur->next;
    cur->next = new Line;
    cur = cur->next;
    cur->p = val;
    cur->next = nxt;
}

void EdgeFind::prependToLine(Line*& root, int val) {
    Line* nRoot = new Line;
    nRoot->p = val;
    nRoot->next = root;
    root = nRoot;
}

void EdgeFind::paintEdges(int *img) {
    paintEdges(img, m_edges);
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

int* EdgeFind::getImageGradients() {
    return m_gradient;
}

int* EdgeFind::getImageDirections() {
    return m_direction;
}

int* EdgeFind::getAnchors() {
    return m_anchors;
}

void** EdgeFind::getImageEdges() {
    return m_edges;
}