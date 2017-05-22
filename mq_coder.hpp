#ifndef MQ_CODER_H
#define	MQ_CODER_H

#include <vector>


class mq_coder {
protected:

	mq_coder() { };

	void init();

	struct state_t {
		const uint16_t qeval;
		const bool mps;
		const state_t * const nmps;
		const state_t * const nlps;

		state_t(const uint32_t& _qeval, const bool& _mps, const state_t * const _nmps, const state_t * const _nlps)
			: qeval(_qeval), mps(_mps), nmps(_nmps), nlps(_nlps) { }
		};

	static state_t states[];

	std::vector<state_t const* > state_mapping;
	};

#endif	/* MQ_CODER_H */
