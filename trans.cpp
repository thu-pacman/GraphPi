#include <cstdio>

int main() {
    long long ans;
    double t1,t2,t3,t4;
    while( scanf("%lld,%lf,%lf,%lf,%lf", &ans, &t1, &t2, &t3, &t4) != EOF) {
        char buf[50];
        for(int i = 0; i < 7; ++i)
            scanf("%s", buf + i * 7);
        printf("7 %s\n", buf);
    }
    return 0;
}
