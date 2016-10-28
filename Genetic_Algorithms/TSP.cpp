#include <bits/stdc++.h>

using namespace std;

int cnt;
int const N = 64;
int const M = 16;
double const MUTATION_RATE = 0.005;

struct city {
  int x, y, id;

  double dist(city& other) {
    return hypot(x-other.x, y-other.y);
  }  
};

city cities[N];

struct tour {
  double dist = -1;
  city a[N];

  tour() {

  }

  tour(int) {
    for(int i = 0; i < N; ++i)
      a[i] = cities[i];

    random_shuffle(a,a+N);
  }

  tour(city cities[]) {
    for(int i = 0; i < N; ++i)
      a[i] = cities[i];
  }

  city& operator[](int idx) {
    return a[idx];
  }

  double get_dist() {
    if(dist < 0) {
      dist = 0;
      for(int i = 1; i < N; ++i)
        dist += a[i-1].dist(a[i]);
    }

    return dist;
  }

  void plot() {
    string cmd;
    cmd.reserve(4096);
    cmd.append("python3 -c 'import matplotlib.pyplot as m; ");

    char title[256];
    sprintf(title, "m.title(\"%.2lf\"); ", dist);
    cmd.append(title);

    for(int i = 1; i < N; ++i) {
      int x1 = a[i-1].x;
      int y1 = a[i-1].y;
      int x2 = a[i].x;
      int y2 = a[i].y;
      char w[256];
      sprintf(w, " m.plot([%d,%d],[%d,%d]);", x1, x2, y1, y2);
      cmd.append(w);
    }

    char last[256];
    sprintf(last, "m.savefig(\"%05d.png\")'", cnt++);
    cmd.append(last);

    system(cmd.c_str());
  }

  bool chk() {    
    bitset<N> used;
    used.reset();
    for(int i = 0; i < N; ++i) {
      if(used[a[i].id])
        return false;
      used[a[i].id] = 1;
    }

  return true;
  }
};

tour cross_over(tour& a, tour& b) {
  tour t;
  int x, y;
  do {
    x = rand() % N;
    y = rand() % N;
  } while(x == y);

  if(x > y)
    swap(x, y);


  bitset<N> used;
  used.reset();
  for(int i = x; i <= y; ++i)
    t[i] = a[i], used[a[i].id] = 1;

  for(int i = 0, idx = 0; i < N; ++i) {
    if(used[b[i].id])
      continue;

    if(idx == x)
      idx = y+1;

    t[idx] = b[i];

    used[b[i].id] = 1;
    idx++;
  }

  return t;
}

void mutate(tour& t) {
  for(int i = 0; i < N; ++i) {
    if(rand() % 100 / 100.0 < MUTATION_RATE) {
      int x;
      do x = rand() % N; while(x == i);
      swap(t[i], t[x]);
    }
  }
}

struct population {
  tour pop[M];
  tour tmp[M];

  population(city cities[]) {
    for(int i = 0; i < M; ++i)
      pop[i] = tour(cities);    
  }

  void evolve() {
    sort(pop, pop + M, [] (tour& a, tour& b) {
      return a.get_dist() < b.get_dist();
    });

    // elites
    tmp[0] = pop[0];
    tmp[1] = pop[1];

    for(int i = 2; i < M; ++i) {
      tmp[i] = cross_over(pop[i], pop[rand() % M]);
      mutate(tmp[i]);
    }

    for(int i = 0; i < M; ++i)
      pop[i] = tmp[i];
  }

  tour& get_fittest() {
    auto fit = 0;
    for(int i = 1; i < M; ++i)
      if(pop[i].get_dist() < pop[fit].get_dist())
        fit = i;
    return pop[fit];
  }
};

int main() {
  cout << fixed;
  srand(time(0));
  int x = 100000 / 2;
  int y = 100000 / 2;
  int r = 100000 / 4;

  for(int i = 0; i < N; ++i) {
    int a = rand() % 100000;
    int b = rand() % 100000;
    a -= x;
    b -= y;
    double mag = hypot(a, b);
    cities[i] = {int(a/mag*r), int(b/mag*r), i};
  }

  population pop(cities);  
  double init_ans = pop.get_fittest().get_dist();

  double best = init_ans;

  int cnt = 0;
  int generations = 1e9;

  for(int i = 0; i < generations; i++) {
    pop.evolve();
    auto&& t = pop.get_fittest();

    if(t.get_dist() < best) {
      cnt++;
      best = t.get_dist();
      cout << cnt << ": " << best << endl;
      t.plot();
    }
  }

  cout << "============================" << endl;
  cout << init_ans << endl;
  cout << best     << endl;
  cout << cnt      << " changes" << endl;
}
