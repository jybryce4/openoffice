/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include <resources.hxx>
#include <WW8ResourceModelImpl.hxx>

namespace writerfilter {
namespace doctok {

void WW8ListTable::initPayload()
{
    sal_uInt32 nCount = getEntryCount();

    sal_uInt32 nOffset = 2;
    sal_uInt32 nOffsetLevel = mnPlcfPayloadOffset;
    for (sal_uInt32 n = 0; n < nCount; ++n)
    {
        WW8List aList(this, nOffset);
        
        entryOffsets.push_back(nOffset);
        payloadIndices.push_back(payloadOffsets.size());
        nOffset += WW8List::getSize();

        sal_uInt32 nLvlCount = aList.get_fSimpleList() ? 1 : 9;

        for (sal_uInt32 i = 0; i < nLvlCount; ++i)
        {
            WW8ListLevel aLevel(this, nOffsetLevel);

            payloadOffsets.push_back(nOffsetLevel);

            nOffsetLevel += aLevel.calcSize();
        }

        if (nOffsetLevel > getCount())
        {
            nOffsetLevel = getCount();

            break;
        }
    }

    payloadOffsets.push_back(nOffsetLevel);
    entryOffsets.push_back(nOffset);    
}

sal_uInt32 WW8ListTable::getEntryCount()
{
    return getU16(0);
}

writerfilter::Reference<Properties>::Pointer_t 
WW8ListTable::getEntry(sal_uInt32 nIndex)
{
    WW8List * pList = new WW8List(this, entryOffsets[nIndex]);

    pList->setIndex(nIndex);

    return writerfilter::Reference<Properties>::Pointer_t
        (pList);
}

sal_uInt32 WW8List::get_listlevel_count()
{
    if (get_fSimpleList())
        return 1;

    return 9;
}

writerfilter::Reference<Properties>::Pointer_t 
WW8List::get_listlevel(sal_uInt32 nIndex)
{
    WW8ListTable * pListTable = dynamic_cast<WW8ListTable *>(mpParent);
    sal_uInt32 nPayloadIndex = pListTable->getPayloadIndex(mnIndex) + nIndex;
    sal_uInt32 nPayloadOffset = pListTable->getPayloadOffset(nPayloadIndex);
    sal_uInt32 nPayloadSize = pListTable->getPayloadSize(nPayloadIndex);
        
    return writerfilter::Reference<Properties>::Pointer_t
        (new WW8ListLevel(mpParent, nPayloadOffset, nPayloadSize));
}

::rtl::OUString WW8ListLevel::get_xst()
{
    sal_uInt32 nOffset = WW8ListLevel::getSize();
    
    nOffset += get_cbGrpprlPapx();
    nOffset += get_cbGrpprlChpx();

    return getString(nOffset);
}

void WW8ListLevel::resolveNoAuto(Properties & rHandler)
{
    sal_uInt32 nOffset = getSize();

    {
        WW8PropertySet::Pointer_t pSet
            (new WW8PropertySetImpl(*this, nOffset, get_cbGrpprlPapx()));

        WW8PropertiesReference aRef(pSet);
        aRef.resolve(rHandler);
    }

    nOffset += get_cbGrpprlPapx();

    {
        WW8PropertySet::Pointer_t pSet
            (new WW8PropertySetImpl(*this, nOffset, get_cbGrpprlChpx()));
    
        WW8PropertiesReference aRef(pSet);
        aRef.resolve(rHandler);
    }
}

sal_uInt32 WW8ListLevel::calcSize()
{
    sal_uInt32 nResult = WW8ListLevel::getSize();

    nResult += get_cbGrpprlPapx();
    nResult += get_cbGrpprlChpx();
    nResult += 2 + getU16(nResult) * 2;

    return nResult;
}
}}
