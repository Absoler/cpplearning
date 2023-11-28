#include <bits/stdc++.h>
#include <cassert>
#include <functional>
#include <vector>
using namespace std;

int choose_pivot_3middle (vector<int> &arr, int lo, int hi) {
    int mid = (lo + hi) >> 1;

    if ( (arr[mid] - arr[lo]) * (arr[mid] - arr[hi]) <= 0 ) {
        return mid;
    } else if ( (arr[lo] - arr[mid]) * (arr[lo] - arr[hi]) <= 0) {
        return lo;
    } else {
        return hi;
    }
}

int lomuto_partition (vector<int> &arr, int lo, int hi) {
    assert(lo <= hi);
    if (lo == hi) {
        return lo;
    }
    // invented by Nico Lomuto
    // * pivot is the last one
    // * like quick-slow pointers

    int pivot_pos = choose_pivot_3middle(arr, lo, hi);
    if (pivot_pos != hi) {
        swap(arr[pivot_pos], arr[hi]);
    }

    int pivot = arr[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j] <= pivot) {
            i += 1;
            swap(arr[i], arr[j]);
        }
    }
    i += 1;
    swap(arr[hi], arr[i]);
    return i;
    // arr[lo, i] <= pivot, arr[i+1, hi] > pivot, arr[i] == pivot
}

int hoare_partition (vector<int> &arr, int lo, int hi) {
    assert(lo <= hi);
    if (lo == hi) {
        return lo;
    }
    // invented by the Tony Hoare
    // * pivot is the middle element

    int pivot = arr[(lo + hi) >> 1];
    int i = lo - 1, j = hi + 1;
    while (true) {
        do {
            i ++;
        }
        while (arr[i] < pivot);
        do {
            j --;
        }
        while (arr[j] > pivot);

        assert(i-j < 2);
        if (i == j || i == j+1) {
            return j;
        }
        swap(arr[i], arr[j]);
    }
    // arr[lo, j] <= pivot, arr[j+1, hi] >= pivot
}


int normal_partition (vector<int> &arr, int lo, int hi) {
    assert(lo <= hi);
    if (lo == hi) {
        return lo;
    }
    // the first way I learnt

    int pivot_pos = choose_pivot_3middle(arr, lo, hi);
    if (pivot_pos != lo) {
        swap(arr[pivot_pos], arr[lo]);
    }

    int pivot = arr[lo];
    int i = lo, j = hi;
    while (i < j) {
        // arr[.., i) <= pivot, arr(j, ..] >= pivot
        while (i < j && arr[j] >= pivot) {
            j--;
        }
        if (i < j) {
            arr[i] = arr[j];
            i++;
        }

        while (i < j && arr[i] <= pivot) {
            i++;
        }
        if (i < j) {
            arr[j] = arr[i];
            j--;
        }
    }
    // i == j when jump out
    assert(i == j);

    arr[i] = pivot;
    return i;
}

function<int(vector<int>&, int, int)>  partition_wrapper;

void quicksort (vector<int> &arr, int lo, int hi) {
    assert(lo <= hi);
    if (lo == hi) {
        return;
    }

    auto *partition_ptr = partition_wrapper.target<int(*)(vector<int>&, int, int)>();
    assert(partition_ptr);
    int mid = partition_wrapper(arr, lo, hi);
    if (*partition_ptr == &lomuto_partition || *partition_ptr == &normal_partition) {
        if (lo < mid) quicksort(arr, lo, mid - 1);
        if (mid < hi) quicksort(arr, mid + 1, hi);
    } else if (*partition_ptr == &hoare_partition) {
        quicksort(arr, lo, mid);
        quicksort(arr, mid + 1, hi);
    }
}

int main (int argc, char *argv[]) {
    if (argc != 2) {
        cout << "need 0, 1, 2 to indicate which partition function" << endl;
        return 1;
    }
    if (argv[1][0] == '0') {
        partition_wrapper = lomuto_partition;
    } else if (argv[1][0] == '1') {
        partition_wrapper = hoare_partition;
    } else if (argv[1][0] == '2') {
        partition_wrapper = normal_partition;
    }
    // vector<int> arr {8, 6, 5, 3, 1};
    // vector<int> arr {3, 4, 2, 1};
    vector<int> arr {2, 3, 1};
    quicksort(arr, 0, arr.size() - 1);
    for (int x : arr) {
        cout << x << ' ';
    }
    cout << endl;
}