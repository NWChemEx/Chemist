/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "chemist/basis_set/primitive/primitive_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace chemist::detail_;

template<typename T, typename U>
void check_state(T&& prim, U&& corr) {
    SECTION("Coefficient") { REQUIRE(prim.coef() == corr[0]); }
    SECTION("Exponent") { REQUIRE(prim.exp() == corr[1]); }
}

TEST_CASE("PrimitivePIMPL<double> : default ctor") {
    PrimitivePIMPL<double> pimpl;
    check_state(pimpl, std::vector<double>(2, 0.0));
}

TEST_CASE("PrimitivePIMPL<double> : copy ctor") {
    SECTION("From owning instance") {
        PrimitivePIMPL<double> p(1.0, 2.0);
        PrimitivePIMPL<double> p2(p);
        check_state(p2, std::vector<double>{1.0, 2.0});
        SECTION("Is a deep copy") {
            REQUIRE(&p.coef() != &p2.coef());
            REQUIRE(&p.exp() != &p2.exp());
        }
    }
    SECTION("From alias instance") {
        std::vector<double> v{1.0, 2.0};
        PrimitivePIMPL<double> p(&v[0], &v[1]);
        PrimitivePIMPL<double> p2(p);
        check_state(p2, v);
        SECTION("Is a deep copy") {
            REQUIRE(&p.coef() != &p2.coef());
            REQUIRE(&p.exp() != &p2.exp());
        }
    }
}

TEST_CASE("PrimitivePIMPL<double> : move ctor") {
    SECTION("From owning instance") {
        PrimitivePIMPL<double> p(1.0, 2.0);
        PrimitivePIMPL<double> p2(std::move(p));
        check_state(p2, std::vector<double>{1.0, 2.0});
        SECTION("RHS is still in the same state") {
            check_state(p, std::vector<double>{1.0, 2.0});
        }
    }
    SECTION("From aliasing instance") {
        std::vector<double> v{1.0, 2.0};
        PrimitivePIMPL<double> p(&v[0], &v[1]);
        PrimitivePIMPL<double> p2(std::move(p));
        check_state(p2, std::vector<double>{1.0, 2.0});
        SECTION("Still aliases") {
            REQUIRE(&p2.coef() == &v[0]);
            REQUIRE(&p2.exp() == &v[1]);
        }
        SECTION("RHS is still in the same state") {
            REQUIRE(&p.coef() == &v[0]);
            REQUIRE(&p.exp() == &v[1]);
        }
    }
}

TEST_CASE("PrimitivePIMPL<double> : copy assignment") {
    SECTION("From owning instance") {
        PrimitivePIMPL<double> p(1.0, 2.0);
        PrimitivePIMPL<double> p2;
        auto pp2 = &(p2 = p);
        check_state(p2, std::vector<double>{1.0, 2.0});
        SECTION("Returns this") { REQUIRE(pp2 == &p2); }
        SECTION("Is a deep copy") {
            REQUIRE(&p.coef() != &p2.coef());
            REQUIRE(&p.exp() != &p2.exp());
        }
    }
    SECTION("From alias instance") {
        std::vector<double> v{1.0, 2.0};
        PrimitivePIMPL<double> p(&v[0], &v[1]);
        PrimitivePIMPL<double> p2;
        auto pp2 = &(p2 = p);
        check_state(p2, v);
        SECTION("Returns this") { REQUIRE(pp2 == &p2); }
        SECTION("Is a deep copy") {
            REQUIRE(&p.coef() != &p2.coef());
            REQUIRE(&p.exp() != &p2.exp());
        }
    }
}

TEST_CASE("PrimitivePIMPL<double> : move assignment") {
    SECTION("From owning instance") {
        PrimitivePIMPL<double> p(1.0, 2.0);
        PrimitivePIMPL<double> p2;
        auto pp2 = &(p2 = std::move(p));
        check_state(p2, std::vector<double>{1.0, 2.0});
        SECTION("Returns this") { REQUIRE(pp2 == &p2); }
        SECTION("RHS is still valid") {
            check_state(p, std::vector<double>{1.0, 2.0});
        }
    }
    SECTION("From aliasing instance") {
        std::vector<double> v{1.0, 2.0};
        PrimitivePIMPL<double> p(&v[0], &v[1]);
        PrimitivePIMPL<double> p2;
        auto pp2 = &(p2 = std::move(p));
        check_state(p2, std::vector<double>{1.0, 2.0});
        SECTION("Returns this") { REQUIRE(pp2 == &p2); }
        SECTION("Still aliases") {
            REQUIRE(&p2.coef() == &v[0]);
            REQUIRE(&p2.exp() == &v[1]);
        }
        SECTION("RHS is still valid") {
            REQUIRE(&p.coef() == &v[0]);
            REQUIRE(&p.exp() == &v[1]);
        }
    }
}

TEST_CASE("PrimitivePIMPL<double> : value ctor") {
    SECTION("Owning") {
        PrimitivePIMPL<double> p(1.0, 2.0);
        check_state(p, std::vector<double>{1.0, 2.0});
    }
    SECTION("Aliasing") {
        std::vector<double> v{1.0, 2.0};
        PrimitivePIMPL<double> p(&v[0], &v[1]);
        REQUIRE(&p.coef() == &v[0]);
        REQUIRE(&p.exp() == &v[1]);
    }
}

TEST_CASE("PrimitivePIMPL<double> : coef") {
    SECTION("Owns") {
        PrimitivePIMPL<double> p(1.0, 2.0);
        REQUIRE(p.coef() == 1.0);
        SECTION("Is read/write") {
            STATIC_REQUIRE(std::is_same_v<double&, decltype(p.coef())>);
        }
    }
    SECTION("Aliases") {
        std::vector<double> v{1.0, 2.0};
        PrimitivePIMPL<double> p(&v[0], &v[1]);
        REQUIRE(&p.coef() == &v[0]);
        SECTION("Is read/write") {
            STATIC_REQUIRE(std::is_same_v<double&, decltype(p.coef())>);
        }
    }
}

TEST_CASE("PrimitivePIMPL<double> : coef const") {
    SECTION("Owns") {
        const PrimitivePIMPL<double> p(1.0, 2.0);
        REQUIRE(p.coef() == 1.0);
        SECTION("Is read-only") {
            STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.coef())>);
        }
    }
    SECTION("Aliases") {
        std::vector<double> v{1.0, 2.0};
        const PrimitivePIMPL<double> p(&v[0], &v[1]);
        REQUIRE(&p.coef() == &v[0]);
        SECTION("Is read-only") {
            STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.coef())>);
        }
    }
}

TEST_CASE("PrimitivePIMPL<double> : exp") {
    SECTION("Owns") {
        PrimitivePIMPL<double> p(1.0, 2.0);
        REQUIRE(p.exp() == 2.0);
        SECTION("Is read/write") {
            STATIC_REQUIRE(std::is_same_v<double&, decltype(p.exp())>);
        }
    }
    SECTION("Aliases") {
        std::vector<double> v{1.0, 2.0};
        PrimitivePIMPL<double> p(&v[0], &v[1]);
        REQUIRE(&p.exp() == &v[1]);
        SECTION("Is read/write") {
            STATIC_REQUIRE(std::is_same_v<double&, decltype(p.exp())>);
        }
    }
}

TEST_CASE("PrimitivePIMPL<double> : exp const") {
    SECTION("Owns") {
        const PrimitivePIMPL<double> p(1.0, 2.0);
        REQUIRE(p.exp() == 2.0);
        SECTION("Is read-only") {
            STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.exp())>);
        }
    }
    SECTION("Aliases") {
        std::vector<double> v{1.0, 2.0};
        const PrimitivePIMPL<double> p(&v[0], &v[1]);
        REQUIRE(&p.exp() == &v[1]);
        SECTION("Is read-only") {
            STATIC_REQUIRE(std::is_same_v<const double&, decltype(p.exp())>);
        }
    }
}
