#pragma once

#include <vector>

namespace sort_utils {

template <typename T, typename Comparator>
void quickSort(std::vector<T>& arr, int left, int right, Comparator cmp) {
    int i = left;
    int j = right;
    T pivot = arr[(left + right) / 2];

    while (i <= j) {
        while (cmp(arr[i], pivot)) i++;
        while (cmp(pivot, arr[j])) j--;

        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    if (left < j) {
        quickSort(arr, left, j, cmp);
    }
    if (i < right) {
        quickSort(arr, i, right, cmp);
    }
}

// OVERLOAD que seus códigos estão tentando usar:
// quickSort(vetor, comparador)
template <typename T, typename Comparator>
void quickSort(std::vector<T>& arr, Comparator cmp) {
    if (arr.empty()) return;
    quickSort(arr, 0, static_cast<int>(arr.size()) - 1, cmp);
}

} // namespace sort_utils
