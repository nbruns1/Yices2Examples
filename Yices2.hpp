#pragma once

#include <list>
#include <sstream>
#include <iostream>
#include <yices.h>


        template <class T>
        struct ObjectCounter {
            static int count;
        };

        template<class T>
        int ObjectCounter<T>::count = 0;

        class Yices2 {
        public:
            typedef term_t result_type;
            typedef std::list<term_t> Exprs;

        private:
            Exprs assumptions_;
            Exprs assertions_;
            bool isPushed_;
            context_t *ctx;

            term_t throw_error(term_t value) {
                if (value == NULL_TERM) {
                    char *error = yices_error_string();
                    std::stringstream ss;
                    ss << "throw_error: ";
                    ss << error;
                    throw std::runtime_error(ss.str());
                }
                return value;
            }

        public:

            Yices2() : isPushed_(false) {
                if (ObjectCounter<Yices2>::count == 0) {
                    yices_init();
                }
                ctx_config_t *config = yices_new_config();
                yices_set_config(config, "mode", "interactive");
                ctx = yices_new_context(config);
                yices_free_config(config);
                ObjectCounter<Yices2>::count++;

            }

            ~Yices2() {
                yices_free_context(ctx);
                ObjectCounter<Yices2>::count--;
                if (ObjectCounter<Yices2>::count == 0) {
                    yices_exit();
                }
            }

            static void print_term(term_t term) {
                int32_t code;

                code = yices_pp_term(stdout, term, UINT32_MAX, 1, 0);
                if (code < 0) {
                    // An error occurred
                    fprintf(stderr, "Error in print_term: ");
                    yices_print_error(stderr);
                    exit(1);
                }
            }

            void assertion(result_type e) {
                assertions_.push_back(e);
            }

            void assumption(result_type e) {
                assumptions_.push_back(e);
            }

            void print_status(smt_status_t status) {
                switch (status) {
                    case STATUS_IDLE:
                        std::cerr << "STATUS_IDLE" << std::endl;
                        break;
                    case STATUS_SEARCHING:
                        std::cerr << "STATUS_SEARCHING" << std::endl;
                        break;
                    case STATUS_UNKNOWN:
                        std::cerr << "STATUS_UNKNOWN" << std::endl;
                        break;
                    case STATUS_SAT:
                        std::cerr << "STATUS_SAT" << std::endl;
                        break;
                    case STATUS_UNSAT:
                        std::cerr << "STATUS_UNSAT" << std::endl;
                        break;
                    case STATUS_INTERRUPTED:
                        std::cerr << "STATUS_INTERRUPTED" << std::endl;
                        break;
                    case STATUS_ERROR:
                        std::cerr << "STATUS_ERROR" << std::endl;
                        break;
                    default:
                        break;
                }
            }

            bool solve() {
                removeOldAssumptions();
                pushAssertions();
                pushAssumptions();
                smt_status_t status = yices_check_context(ctx, NULL);
                print_status(status);
                return (status == STATUS_SAT);
            }

        private:

            void removeOldAssumptions() {
                printf("removeOldAssumptions\n");
                if (isPushed_) {
                    if (yices_pop(ctx) == -1) {
                        char *error = yices_error_string();
                        std::stringstream ss;
                        ss << "removeOldAssumptions Error: ";
                        ss << error;
                        throw std::runtime_error(ss.str());
                    }
                    isPushed_ = false;
                }
            }

            void pushAssumptions() {
                printf("pushAssumptions\n");
				smt_status_t status = yices_context_status(ctx);
				if(status != STATUS_IDLE && status != STATUS_SAT && status != STATUS_UNKNOWN)
				{
					printf("invalid State Status: ");
					print_status(status);
					return;
				}
                if (yices_push(ctx) == -1) {
                    print_status(yices_context_status(ctx));
                    char *error = yices_error_string();
                    std::stringstream ss;
                    ss << "pushAssumptions Error: ";
                    ss << error;
                    throw std::runtime_error(ss.str());
                } else {
                    isPushed_ = true;
                    applyAssertions(assumptions_);
                    assumptions_.clear();
                }
            }

            void pushAssertions() {
                printf("pushAssertions\n");
                applyAssertions(assertions_);
                assertions_.clear();
            }

            void applyAssertions(Exprs const &expressions) {
                for (Exprs::const_iterator it = expressions.begin(), ie = expressions.end(); it != ie; ++it) {
                    print_term(*it);
                    if(yices_assert_formula(ctx, *it) != 0)
					{
						char *error = yices_error_string();
						std::string s;
						s += "applyAssertions0: ";
						s += error;
					}
                }
            }

        }; // class Yices2
