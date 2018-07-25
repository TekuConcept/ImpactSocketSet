/**
 * Created by TekuConcept on July 25, 2017
 */

#ifndef _IMPACT_RFC_URI_H_
#define _IMPACT_RFC_URI_H_

#include <string>
#include <map>

namespace impact {
	class uri {
	public:
		uri(const std::string& uri);

		std::string    scheme()   const;
		std::string    host()     const;
		std::string    resource() const;
		unsigned short port()     const;
		virtual bool   secure()   const;

		static bool    valid(const std::string& uri);
		static uri     parse(const std::string& uri);
		static uri     try_parse(const std::string& uri, bool& success);

		static void    register_scheme(
			const std::string& name, unsigned short port, bool secure = false);

	protected:
		std::string    m_scheme_;
		std::string    m_host_;
		std::string    m_resource_;
		unsigned short m_port_;
		bool           m_secure_;

	private:
		static std::map<std::string, std::pair<bool, unsigned short>>
			s_scheme_meta_data_;

		uri();

		static bool _S_parse(const std::string& u, uri& result);
		static bool _S_parse_scheme(const std::string& u, uri& result);
		static void _S_set_meta_info(uri& result);
		static bool _S_parse_ipv6_host(
			const std::string& u, unsigned int& offset, uri& result);
		static bool _S_parse_host(
			const std::string& u, unsigned int& offset, uri& result);
		static bool _S_parse_port(
			const std::string& u, unsigned int& offset, uri& result);
	};
}

#endif