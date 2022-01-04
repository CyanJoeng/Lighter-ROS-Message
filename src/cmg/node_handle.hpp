/**
 * Author: Cyan
 * Date: Thu Dec  2 17:37:35 CST 2021
 */
#pragma once
#include <cstdio>
#include <exception>
#include <functional>
#include <stdexcept>
#include <string>
#include <memory>
#include <list>
#include <vector>

#include <boost/algorithm/string.hpp>


#include "enviroment.hpp"
#include "receiver.hpp"
#include "utils/config.hpp"

namespace cmg {


	class NodeHandle {

	public:
		class NodePublisher {

			std::shared_ptr<Sender> sender_;

		public:
			NodePublisher() = default;

			NodePublisher(const std::shared_ptr<Sender> &sender);

			auto publish(const Message &msg) -> bool;
		};

		class NodeSubscriber {

			std::shared_ptr<Receiver> receiver_;

		public:
			NodeSubscriber() = default;

			NodeSubscriber(const std::shared_ptr<Receiver> &receiver);
		};

	private:
		std::string proc_name_;

		const cmg::Config &config_;

	public:
		NodeHandle(const std::string &proc_connect);

		void shutdown();

	public:
		template <typename Msg>
		auto advertise(const std::string &topic, unsigned wait) -> NodePublisher {

			return NodePublisher(Environment::Inst(this->proc_name_).sender<Msg>(topic, wait));
		}

		template <typename Msg>
		auto subscribe(const std::string &proc_name_topic, unsigned wait, void (*callback)(const std::shared_ptr<const Msg>&)) -> NodeSubscriber {

			printf("subscribe proc_topic %s\n", proc_name_topic.c_str());

			std::vector<std::string> strs;
//			boost::split(strs, proc_name_topic, boost::is_any_of("/"));

            std::string str = proc_name_topic;
            while (!str.empty()) {

            	auto pos = str.find('/');
				if (pos != -1) {

					strs.push_back(str.substr(0, pos));
					str = str.substr(pos + 1);
				} else {
				
					strs.push_back(str);
					break;
				}
            }

			auto &server_proc_name = strs[1];
			auto &topic = strs.back();

			printf("subscribe to %s/%s\n", server_proc_name.c_str(), topic.c_str());

			return NodeSubscriber(Environment::Inst(server_proc_name).receiver<Msg>(topic, wait, callback));
		}

		template <typename T>
		auto getParam(const std::string &key, T &val) -> bool {

			try {

				this->config_.get<T>(key, val);
				val = this->config_.get<T>(key);
			} catch (const std::exception &e) {

				printf("[NodeHandle]getParam error %s %s\n", key.c_str(), typeid(T).name());
				throw std::runtime_error(e.what());
			}
			return true;
		}
	};


	using Publisher = NodeHandle::NodePublisher;

	using Subscriber = NodeHandle::NodeSubscriber;
}
