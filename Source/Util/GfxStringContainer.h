#ifndef __INCLUDE_GFXSTRINGCONTAINER_H__
#define __INCLUDE_GFXSTRINGCONTAINER_H__


#include <list>
#include <string>


namespace GfxLib
{


    class StringContainer
    {
    public:
        LPCWSTR Copy(LPCWSTR string)
        {
            if (string)
            {
                m_Strings.push_back(string);
                return m_Strings.back().c_str();
            } else
            {
                return nullptr;
            }
        }
        void clear() { m_Strings.clear(); }
    private:
        std::list<std::wstring> m_Strings;
    };




}



#endif //__INCLUDE_GFXSTRINGCONTAINER_H__