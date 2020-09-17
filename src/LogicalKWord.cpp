#include <query/LogicalOperator.h>
#include <query/Query.h>


namespace scidb
{

class LogicalKWord : public LogicalOperator
{
public:
    LogicalKWord(const std::string& logicalName, const std::string& alias):
        LogicalOperator(logicalName, alias)
    {
    }

    static PlistSpec const* makePlistSpec()
    {
        static PlistSpec argSpec {
            { "foo", RE(PP(PLACEHOLDER_CONSTANT, TID_STRING)) },
            { "bar", RE(PP(PLACEHOLDER_CONSTANT, TID_STRING)) },
        };
        return &argSpec;
    }

    ArrayDesc inferSchema(std::vector< ArrayDesc> schemas, std::shared_ptr< Query> query)
    {
        Attributes attributes;
        attributes.push_back(AttributeDesc("foo", TID_STRING, 0, CompressorType::NONE));

        return ArrayDesc("kword",
                         attributes,
                         std::vector<DimensionDesc>(
                             1,
                             DimensionDesc("bar", 0, 0, 0, 0, 1, 0)),
                         createDistribution(getSynthesizedDistType()),
                         query->getDefaultArrayResidency());
    }
};

REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalKWord, "kword");

} // end namespace scidb
