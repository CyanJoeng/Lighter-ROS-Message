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

#include "cmg/core/environment.hpp"
#include "cmg/core/receiver.hpp"
#include "cmg/utils/config.hpp"

namespace cmg {


	class NodeHandle {

	public:
		class NodePublisher {

			std::shared_ptr<Sender> sender_;

		public:
			NodePublisher() = default;

			NodePublisher(std::shared_ptr<Sender> sender);

			auto publish(const Message &msg) -> bool;
		};

		class NodeSubscriber {

			std::shared_ptr<Receiver> receiver_;

		public:
			NodeSubscriber() = default;

			NodeSubscriber(std::shared_ptr<Receiver> receiver);
		};

	private:
		Environment &env_;

		cmg::Config &config_;

	public:
		NodeHandle(const std::string& proc_connect_to);

		void shutdown();

	public:
		template <typename Msg>
		auto advertise(const std::string &topic, unsigned wait) -> NodePublisher {

			return NodePublisher(this->env_.sender<Msg>(topic, wait));
		}

		template <typename Msg>
		auto subscribe(const std::string &proc_name_topic, unsigned wait, void (*callback)(const std::shared_ptr<const Msg>&)) -> NodeSubscriber {

			return NodeSubscriber(this->env_.receiver<Msg>(proc_name_topic, wait, callback));
		}

		template <typename T>
		auto getParam(const std::string &key, T &val) const -> bool {

			try {

				this->config_.get<T>(key, val);
			} catch (const std::exception &e) {

				CMG_WARN("[NodeHandle]getParam error %s %s", key.c_str(), typeid(T).name());
				throw std::runtime_error(e.what());
			}
			return true;
		}

		template <typename T>
		void setParam(const std::string &key, const T &val) {

			this->config_.set(key, val);
		}
	};


	using Publisher = NodeHandle::NodePublisher;

	using Subscriber = NodeHandle::NodeSubscriber;
}
