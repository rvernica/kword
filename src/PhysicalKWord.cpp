#include <array/TileIteratorAdaptors.h>
#include <query/PhysicalOperator.h>


namespace scidb
{

class PhysicalKWord : public PhysicalOperator
{
public:
    PhysicalKWord(std::string const& logicalName,
                   std::string const& physicalName,
                   Parameters const& parameters,
                   ArrayDesc const& schema):
        PhysicalOperator(logicalName, physicalName, parameters, schema)
    {}

    std::shared_ptr<Array> execute(
        std::vector< std::shared_ptr<Array> >& inputArrays,
        std::shared_ptr<Query> query)
    {
        std::shared_ptr<Array> array = std::make_shared<MemArray>(_schema, query);
        return array;
    }
};

REGISTER_PHYSICAL_OPERATOR_FACTORY(PhysicalKWord, "kword", "PhysicalKWord");

} // end namespace scidb
