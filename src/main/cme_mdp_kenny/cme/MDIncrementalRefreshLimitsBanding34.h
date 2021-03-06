/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _MKTDATA_MDINCREMENTALREFRESHLIMITSBANDING34_H_
#define _MKTDATA_MDINCREMENTALREFRESHLIMITSBANDING34_H_

#if defined(SBE_HAVE_CMATH)
/* cmath needed for std::numeric_limits<double>::quiet_NaN() */
#  include <cmath>
#  define SBE_FLOAT_NAN std::numeric_limits<float>::quiet_NaN()
#  define SBE_DOUBLE_NAN std::numeric_limits<double>::quiet_NaN()
#else
/* math.h needed for NAN */
#  include <math.h>
#  define SBE_FLOAT_NAN NAN
#  define SBE_DOUBLE_NAN NAN
#endif

#if __cplusplus >= 201103L
#  include <cstdint>
#  include <functional>
#  include <string>
#  include <cstring>
#endif

#include <sbe/sbe.h>

#include "MDEntryTypeBook.h"
#include "OpenCloseSettlFlag.h"
#include "SecurityTradingStatus.h"
#include "LegSide.h"
#include "MatchEventIndicator.h"
#include "MaturityMonthYear.h"
#include "FLOAT.h"
#include "MDEntryTypeDailyStatistics.h"
#include "PRICE.h"
#include "PutOrCall.h"
#include "EventType.h"
#include "SecurityUpdateAction.h"
#include "SecurityTradingEvent.h"
#include "DecimalQty.h"
#include "MDEntryTypeStatistics.h"
#include "MDUpdateAction.h"
#include "InstAttribValue.h"
#include "AggressorSide.h"
#include "GroupSize8Byte.h"
#include "HaltReason.h"
#include "GroupSize.h"
#include "SettlPriceType.h"
#include "MDEntryType.h"
#include "PRICENULL.h"

using namespace sbe;

namespace mktdata {

class MDIncrementalRefreshLimitsBanding34
{
private:
    char *m_buffer;
    std::uint64_t m_bufferLength;
    std::uint64_t *m_positionPtr;
    std::uint64_t m_offset;
    std::uint64_t m_position;
    std::uint64_t m_actingBlockLength;
    std::uint64_t m_actingVersion;

    inline void reset(
        char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength,
        const std::uint64_t actingBlockLength, const std::uint64_t actingVersion)
    {
        m_buffer = buffer;
        m_offset = offset;
        m_bufferLength = bufferLength;
        m_actingBlockLength = actingBlockLength;
        m_actingVersion = actingVersion;
        m_positionPtr = &m_position;
        position(offset + m_actingBlockLength);
    }

public:

    MDIncrementalRefreshLimitsBanding34(void) : m_buffer(nullptr), m_bufferLength(0), m_offset(0) {}

    MDIncrementalRefreshLimitsBanding34(char *buffer, const std::uint64_t bufferLength)
    {
        reset(buffer, 0, bufferLength, sbeBlockLength(), sbeSchemaVersion());
    }

    MDIncrementalRefreshLimitsBanding34(char *buffer, const std::uint64_t bufferLength, const std::uint64_t actingBlockLength, const std::uint64_t actingVersion)
    {
        reset(buffer, 0, bufferLength, actingBlockLength, actingVersion);
    }

    MDIncrementalRefreshLimitsBanding34(const MDIncrementalRefreshLimitsBanding34& codec)
    {
        reset(codec.m_buffer, codec.m_offset, codec.m_bufferLength, codec.m_actingBlockLength, codec.m_actingVersion);
    }

#if __cplusplus >= 201103L
    MDIncrementalRefreshLimitsBanding34(MDIncrementalRefreshLimitsBanding34&& codec)
    {
        reset(codec.m_buffer, codec.m_offset, codec.m_bufferLength, codec.m_actingBlockLength, codec.m_actingVersion);
    }

    MDIncrementalRefreshLimitsBanding34& operator=(MDIncrementalRefreshLimitsBanding34&& codec)
    {
        reset(codec.m_buffer, codec.m_offset, codec.m_bufferLength, codec.m_actingBlockLength, codec.m_actingVersion);
        return *this;
    }

#endif

    MDIncrementalRefreshLimitsBanding34& operator=(const MDIncrementalRefreshLimitsBanding34& codec)
    {
        reset(codec.m_buffer, codec.m_offset, codec.m_bufferLength, codec.m_actingBlockLength, codec.m_actingVersion);
        return *this;
    }

    static const std::uint16_t sbeBlockLength(void)
    {
        return (std::uint16_t)11;
    }

    static const std::uint16_t sbeTemplateId(void)
    {
        return (std::uint16_t)34;
    }

    static const std::uint16_t sbeSchemaId(void)
    {
        return (std::uint16_t)1;
    }

    static const std::uint16_t sbeSchemaVersion(void)
    {
        return (std::uint16_t)6;
    }

    static const char *sbeSemanticType(void)
    {
        return "X";
    }

    std::uint64_t offset(void) const
    {
        return m_offset;
    }

    MDIncrementalRefreshLimitsBanding34 &wrapForEncode(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        reset(buffer, offset, bufferLength, sbeBlockLength(), sbeSchemaVersion());
        return *this;
    }

    MDIncrementalRefreshLimitsBanding34 &wrapForDecode(
         char *buffer, const std::uint64_t offset, const std::uint64_t actingBlockLength,
         const std::uint64_t actingVersion, const std::uint64_t bufferLength)
    {
        reset(buffer, offset, bufferLength, actingBlockLength, actingVersion);
        return *this;
    }

    std::uint64_t position(void) const
    {
        return m_position;
    }

    void position(const std::uint64_t position)
    {
        if (SBE_BOUNDS_CHECK_EXPECT((position > m_bufferLength), false))
        {
            throw std::runtime_error("buffer too short [E100]");
        }
        m_position = position;
    }

    std::uint64_t encodedLength(void) const
    {
        return position() - m_offset;
    }

    char *buffer(void)
    {
        return m_buffer;
    }

    std::uint64_t actingVersion(void) const
    {
        return m_actingVersion;
    }

    static const std::uint16_t transactTimeId(void)
    {
        return 60;
    }

    static const std::uint64_t transactTimeSinceVersion(void)
    {
         return 0;
    }

    bool transactTimeInActingVersion(void)
    {
        return (m_actingVersion >= transactTimeSinceVersion()) ? true : false;
    }


    static const char *TransactTimeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "UTCTimestamp";
        }

        return "";
    }

    static const std::uint64_t transactTimeNullValue()
    {
        return SBE_NULLVALUE_UINT64;
    }

    static const std::uint64_t transactTimeMinValue()
    {
        return 0x0L;
    }

    static const std::uint64_t transactTimeMaxValue()
    {
        return 0xfffffffffffffffeL;
    }

    std::uint64_t transactTime(void) const
    {
        return SBE_LITTLE_ENDIAN_ENCODE_64(*((std::uint64_t *)(m_buffer + m_offset + 0)));
    }

    MDIncrementalRefreshLimitsBanding34 &transactTime(const std::uint64_t value)
    {
        *((std::uint64_t *)(m_buffer + m_offset + 0)) = SBE_LITTLE_ENDIAN_ENCODE_64(value);
        return *this;
    }

    static const std::uint16_t matchEventIndicatorId(void)
    {
        return 5799;
    }

    static const std::uint64_t matchEventIndicatorSinceVersion(void)
    {
         return 0;
    }

    bool matchEventIndicatorInActingVersion(void)
    {
        return (m_actingVersion >= matchEventIndicatorSinceVersion()) ? true : false;
    }


    static const char *MatchEventIndicatorMetaAttribute(const MetaAttribute::Attribute metaAttribute)
    {
        switch (metaAttribute)
        {
            case MetaAttribute::EPOCH: return "unix";
            case MetaAttribute::TIME_UNIT: return "nanosecond";
            case MetaAttribute::SEMANTIC_TYPE: return "MultipleCharValue";
        }

        return "";
    }

private:
    MatchEventIndicator m_matchEventIndicator;

public:

    MatchEventIndicator &matchEventIndicator()
    {
        m_matchEventIndicator.wrap(m_buffer, m_offset + 8, m_actingVersion, m_bufferLength);
        return m_matchEventIndicator;
    }

    class NoMDEntries
    {
    private:
        char *m_buffer;
        std::uint64_t m_bufferLength;
        std::uint64_t *m_positionPtr;
        std::uint64_t m_blockLength;
        std::uint64_t m_count;
        std::uint64_t m_index;
        std::uint64_t m_offset;
        std::uint64_t m_actingVersion;
        GroupSize m_dimensions;

    public:

        inline void wrapForDecode(char *buffer, std::uint64_t *pos, const std::uint64_t actingVersion, const std::uint64_t bufferLength)
        {
            m_buffer = buffer;
            m_bufferLength = bufferLength;
            m_dimensions.wrap(m_buffer, *pos, actingVersion, bufferLength);
            m_blockLength = m_dimensions.blockLength();
            m_count = m_dimensions.numInGroup();
            m_index = -1;
            m_actingVersion = actingVersion;
            m_positionPtr = pos;
            *m_positionPtr = *m_positionPtr + 3;
        }

        inline void wrapForEncode(char *buffer, const std::uint8_t count, std::uint64_t *pos, const std::uint64_t actingVersion, const std::uint64_t bufferLength)
        {
            m_buffer = buffer;
            m_bufferLength = bufferLength;
            m_dimensions.wrap(m_buffer, *pos, actingVersion, bufferLength);
            m_dimensions.blockLength((std::uint16_t)32);
            m_dimensions.numInGroup((std::uint8_t)count);
            m_index = -1;
            m_count = count;
            m_blockLength = 32;
            m_actingVersion = actingVersion;
            m_positionPtr = pos;
            *m_positionPtr = *m_positionPtr + 3;
        }

        static const std::uint64_t sbeHeaderSize()
        {
            return 3;
        }

        static const std::uint64_t sbeBlockLength()
        {
            return 32;
        }

        std::uint64_t position(void) const
        {
            return *m_positionPtr;
        }

        void position(const std::uint64_t position)
        {
            if (SBE_BOUNDS_CHECK_EXPECT((position > m_bufferLength), false))
            {
                 throw std::runtime_error("buffer too short [E100]");
            }
            *m_positionPtr = position;
        }

        inline std::uint64_t count(void) const
        {
            return m_count;
        }

        inline bool hasNext(void) const
        {
            return m_index + 1 < m_count;
        }

        inline NoMDEntries &next(void)
        {
            m_offset = *m_positionPtr;
            if (SBE_BOUNDS_CHECK_EXPECT(( (m_offset + m_blockLength) > m_bufferLength ), false))
            {
                throw std::runtime_error("buffer too short to support next group index [E108]");
            }
            *m_positionPtr = m_offset + m_blockLength;
            ++m_index;

            return *this;
        }

    #if __cplusplus < 201103L
        template<class Func> inline void forEach(Func& func)
        {
            while(hasNext())
            {
                next(); func(*this);
            }
        }

    #else
        inline void forEach(std::function<void(NoMDEntries&)> func)
        {
            while(hasNext())
            {
                next(); func(*this);
            }
        }

    #endif


        static const std::uint16_t highLimitPriceId(void)
        {
            return 1149;
        }

        static const std::uint64_t highLimitPriceSinceVersion(void)
        {
             return 0;
        }

        bool highLimitPriceInActingVersion(void)
        {
            return (m_actingVersion >= highLimitPriceSinceVersion()) ? true : false;
        }


        static const char *HighLimitPriceMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "Price";
            }

            return "";
        }

private:
        PRICENULL m_highLimitPrice;

public:

        PRICENULL &highLimitPrice(void)
        {
            m_highLimitPrice.wrap(m_buffer, m_offset + 0, m_actingVersion, m_bufferLength);
            return m_highLimitPrice;
        }

        static const std::uint16_t lowLimitPriceId(void)
        {
            return 1148;
        }

        static const std::uint64_t lowLimitPriceSinceVersion(void)
        {
             return 0;
        }

        bool lowLimitPriceInActingVersion(void)
        {
            return (m_actingVersion >= lowLimitPriceSinceVersion()) ? true : false;
        }


        static const char *LowLimitPriceMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "Price";
            }

            return "";
        }

private:
        PRICENULL m_lowLimitPrice;

public:

        PRICENULL &lowLimitPrice(void)
        {
            m_lowLimitPrice.wrap(m_buffer, m_offset + 8, m_actingVersion, m_bufferLength);
            return m_lowLimitPrice;
        }

        static const std::uint16_t maxPriceVariationId(void)
        {
            return 1143;
        }

        static const std::uint64_t maxPriceVariationSinceVersion(void)
        {
             return 0;
        }

        bool maxPriceVariationInActingVersion(void)
        {
            return (m_actingVersion >= maxPriceVariationSinceVersion()) ? true : false;
        }


        static const char *MaxPriceVariationMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "Price";
            }

            return "";
        }

private:
        PRICENULL m_maxPriceVariation;

public:

        PRICENULL &maxPriceVariation(void)
        {
            m_maxPriceVariation.wrap(m_buffer, m_offset + 16, m_actingVersion, m_bufferLength);
            return m_maxPriceVariation;
        }

        static const std::uint16_t securityIDId(void)
        {
            return 48;
        }

        static const std::uint64_t securityIDSinceVersion(void)
        {
             return 0;
        }

        bool securityIDInActingVersion(void)
        {
            return (m_actingVersion >= securityIDSinceVersion()) ? true : false;
        }


        static const char *SecurityIDMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static const std::int32_t securityIDNullValue()
        {
            return SBE_NULLVALUE_INT32;
        }

        static const std::int32_t securityIDMinValue()
        {
            return -2147483647;
        }

        static const std::int32_t securityIDMaxValue()
        {
            return 2147483647;
        }

        std::int32_t securityID(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::int32_t *)(m_buffer + m_offset + 24)));
        }

        NoMDEntries &securityID(const std::int32_t value)
        {
            *((std::int32_t *)(m_buffer + m_offset + 24)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
            return *this;
        }

        static const std::uint16_t rptSeqId(void)
        {
            return 83;
        }

        static const std::uint64_t rptSeqSinceVersion(void)
        {
             return 0;
        }

        bool rptSeqInActingVersion(void)
        {
            return (m_actingVersion >= rptSeqSinceVersion()) ? true : false;
        }


        static const char *RptSeqMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static const std::uint32_t rptSeqNullValue()
        {
            return SBE_NULLVALUE_UINT32;
        }

        static const std::uint32_t rptSeqMinValue()
        {
            return 0;
        }

        static const std::uint32_t rptSeqMaxValue()
        {
            return 4294967293;
        }

        std::uint32_t rptSeq(void) const
        {
            return SBE_LITTLE_ENDIAN_ENCODE_32(*((std::uint32_t *)(m_buffer + m_offset + 28)));
        }

        NoMDEntries &rptSeq(const std::uint32_t value)
        {
            *((std::uint32_t *)(m_buffer + m_offset + 28)) = SBE_LITTLE_ENDIAN_ENCODE_32(value);
            return *this;
        }

        static const std::uint16_t mDUpdateActionId(void)
        {
            return 279;
        }

        static const std::uint64_t mDUpdateActionSinceVersion(void)
        {
             return 0;
        }

        bool mDUpdateActionInActingVersion(void)
        {
            return (m_actingVersion >= mDUpdateActionSinceVersion()) ? true : false;
        }


        static const char *MDUpdateActionMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "int";
            }

            return "";
        }

        static const std::int8_t mDUpdateActionNullValue()
        {
            return SBE_NULLVALUE_INT8;
        }

        static const std::int8_t mDUpdateActionMinValue()
        {
            return (std::int8_t)-127;
        }

        static const std::int8_t mDUpdateActionMaxValue()
        {
            return (std::int8_t)127;
        }

        std::int8_t mDUpdateAction(void) const
        {
            return (std::int8_t)0;
        }

        static const std::uint16_t mDEntryTypeId(void)
        {
            return 269;
        }

        static const std::uint64_t mDEntryTypeSinceVersion(void)
        {
             return 0;
        }

        bool mDEntryTypeInActingVersion(void)
        {
            return (m_actingVersion >= mDEntryTypeSinceVersion()) ? true : false;
        }


        static const char *MDEntryTypeMetaAttribute(const MetaAttribute::Attribute metaAttribute)
        {
            switch (metaAttribute)
            {
                case MetaAttribute::EPOCH: return "unix";
                case MetaAttribute::TIME_UNIT: return "nanosecond";
                case MetaAttribute::SEMANTIC_TYPE: return "char";
            }

            return "";
        }

        static const char mDEntryTypeNullValue()
        {
            return (char)0;
        }

        static const char mDEntryTypeMinValue()
        {
            return (char)32;
        }

        static const char mDEntryTypeMaxValue()
        {
            return (char)126;
        }

        static const std::uint64_t mDEntryTypeLength(void)
        {
            return 1;
        }

        const char *mDEntryType(void) const
        {
            static sbe_uint8_t mDEntryTypeValues[] = {103};

            return (const char *)mDEntryTypeValues;
        }

        char mDEntryType(const std::uint64_t index) const
        {
            static sbe_uint8_t mDEntryTypeValues[] = {103};

            return mDEntryTypeValues[index];
        }

        std::uint64_t getMDEntryType(char *dst, const std::uint64_t length) const
        {
            static sbe_uint8_t mDEntryTypeValues[] = {103};
            std::uint64_t bytesToCopy = (length < sizeof(mDEntryTypeValues)) ? length : sizeof(mDEntryTypeValues);

            std::memcpy(dst, mDEntryTypeValues, bytesToCopy);
            return bytesToCopy;
        }
    };

private:
    NoMDEntries m_noMDEntries;

public:

    static const std::uint16_t NoMDEntriesId(void)
    {
        return 268;
    }


    inline NoMDEntries &noMDEntries(void)
    {
        m_noMDEntries.wrapForDecode(m_buffer, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noMDEntries;
    }

    NoMDEntries &noMDEntriesCount(const std::uint8_t count)
    {
        m_noMDEntries.wrapForEncode(m_buffer, count, m_positionPtr, m_actingVersion, m_bufferLength);
        return m_noMDEntries;
    }
};
}
#endif
