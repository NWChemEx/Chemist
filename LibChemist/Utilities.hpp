#pragma once

namespace LibChemist {

/** \brief Computes the number of ways to choose \p k objects from \p n objects.
 *
 * \note This is not the best way to compute a BC nor can it handle non-standard
 *       BCs such as negative or non-integer numbers.
 *
 * \param[in] n The top number in the binomial coefficient corresponding to the
 *              total number of objects in a combinatorial sense.
 * \param[in] k The bottom number in the binomial coefficient corresponding to
 *               the number of objects selected at a time.
 *
 * \return The requested binomial coeffiicient.
 * \tparam The type of the two input integers as well as the output.
 */
template<typename T>
T binomial_coefficient(T n, T k) {
    if(k == 0 || k == n) return 1;
    if(k > n || k < 0 || n < 0) return 0;
    return binomial_coefficient(n - 1, k - 1) + binomial_coefficient(n - 1, k);
}

/** \brief Computes the number of ways to take \p k objects from \p n objects
 *         when repetition is allowed.
 *
 * \note As this is a thin wrapper over binomial_coefficient the same domain
 *       restrictions apply.
 *
 * \param[in] n The top number of the multinomial coefficient corresponding to
 *              the number of objects in a combinatorial sense.
 * \param[in] k The bottom number in the multinomial coefficient corresponding
 *              to the number of objects selected at a time.
 *
 * \returns The requested multinomial coefficient.
 * \tparam T The type of the two input integers as well as the output.
 *
 */
template<typename T>
T multinomial_coefficient(T n, T k) {
    return binomial_coefficient(n + k - 1, k);
}

} // namespace LibChemist
