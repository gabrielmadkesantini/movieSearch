#pragma once

#include <vector>

namespace sort_utils {

//o template Comparator é a função que define o critério de desempate
template <typename T, typename Comparator>
void quickSort(std::vector<T>& arr, int left, int right, Comparator cmp) {
    int i = left;
    int j = right;
    T pivot = arr[(left + right) / 2]; //escolhe o pivô como o elemento do meio

    // Particionamento
    while (i <= j) {
        //enquanto o critério de comparação indicar que arr[i] < pivot, avança i
        while (cmp(arr[i], pivot)) i++;
        while (cmp(pivot, arr[j])) j--;

        if (i <= j) {
            // Troca os elementos em i e j
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }


    // Chamada recursiva para as duas metades
    if (left < j) {
        quickSort(arr, left, j, cmp);
    }
    if (i < right) {
        quickSort(arr, i, right, cmp);
    }
}


// quickSort(vetor, comparador)
template <typename T, typename Comparator>
void quickSort(std::vector<T>& arr, Comparator cmp) {
    if (arr.empty()) return;
    quickSort(arr, 0, static_cast<int>(arr.size()) - 1, cmp);
}

} // namespace sort_utils
