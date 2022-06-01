#include "mapping.hpp"

// TODO optimize
/**
 * Extract the possible mappings whose key_sequence verify the string
 *
 * @param s : The string to query for
 * @param mappings : The mapping set to query
 * @param extracted : The resulting set
 *
 * @return The position in the string from which the first mapping could be extracted
 */
int Mappings::extract(std::string *s, std::set<Mapping, Mappings::mappingSort> &mappings,
                      std::set<Mapping, Mappings::mappingSort> *extracted) {
    bool found = false;
    int i;
    for (i = 0; i < s->size(); i++) {
        std::string currentQuery = s->substr(i, s->size() - i);
        for (auto &m : mappings) {
            if (m.key_sequence.starts_with(currentQuery)) {
                extracted->emplace(m);
                found = true;
            }
        }
        if (found)
            return i;
    }
    return -1;
}
