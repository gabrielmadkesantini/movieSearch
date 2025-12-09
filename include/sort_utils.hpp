#pragma once

#include <vector>

namespace sort_utils {

template <typename T, typename Comparator>
void quickSort(std::vector<T>& arr, int left, int right, Comparator cmp) {
    if (left >= right) return;

    int i = left;
    int j = right;
    T pivot = arr[(left + right) / 2];

    while (i <= j) {
        while (cmp(arr[i], pivot) < 0) i++;
        while (cmp(arr[j], pivot) > 0) j--;

        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    quickSort(arr, left, j, cmp);
    quickSort(arr, i, right, cmp);
}

}
