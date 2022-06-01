#ifndef MAPPING_HPP
#define MAPPING_HPP

#include <set>
#include <string>

class Miller;


typedef void (*ActionFunction)(Miller *);


struct Mapping {
    std::string key_sequence;
    ActionFunction action;
    std::string description;  // description to be printed when help is asked or multiple
                              // options
};


namespace Mappings {
struct mappingSort {
    bool operator()(const Mapping &a, const Mapping &b) const {
        return a.key_sequence > b.key_sequence;
    }
};
int extract(std::string *s, std::set<Mapping, mappingSort> &mappings,
            std::set<Mapping, mappingSort> *extracted);
};  // namespace Mappings

typedef std::set<Mapping, Mappings::mappingSort> MappingSet;

#endif  // MAPPING_HPP
