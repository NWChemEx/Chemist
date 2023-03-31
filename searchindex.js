Search.setIndex({"docnames": ["developer/design/index", "developer/design/vector_spaces", "developer/design/wavefunction", "developer/index", "index", "sparse_maps/background", "sparse_maps/design", "sparse_maps/from_sparse_map", "sparse_maps/index"], "filenames": ["developer/design/index.rst", "developer/design/vector_spaces.rst", "developer/design/wavefunction.rst", "developer/index.rst", "index.rst", "sparse_maps/background.rst", "sparse_maps/design.rst", "sparse_maps/from_sparse_map.rst", "sparse_maps/index.rst"], "titles": ["Design Documentation", "Vector Space Design", "Wavefunction Design", "Developer Documentation", "Chemist", "SparseMap Background", "Sparse Map Library Design", "Sparsifying a Tensor", "Sparse Maps Sublibrary"], "terms": {"topic": [0, 3], "thi": [0, 1, 3, 6, 7], "section": [0, 1, 3], "record": 0, "process": 0, "chemist": [0, 1, 3], "librari": [0, 4, 8], "vector": [0, 3, 5], "space": [0, 3, 5], "what": 0, "ar": [0, 3, 5, 6, 7], "why": 0, "do": 0, "we": [0, 5, 6, 7], "need": [0, 6], "consider": [0, 8], "summari": 0, "futur": 0, "wavefunct": [0, 3], "page": [1, 7], "document": [1, 4], "compon": 1, "In": [1, 5, 7], "practic": [1, 5], "realli": 1, "focus": 1, "basi": 1, "set": [1, 6, 7], "defin": [1, 7], "howev": 1, "quantum": 1, "chemistri": 1, "term": [1, 5], "i": [1, 5, 6, 7], "usual": 1, "associ": [1, 7], "specif": 1, "quantiti": 1, "like": [1, 6], "6": 1, "31g": 1, "wish": 1, "avoid": [1, 6], "confus": 1, "One": 1, "can": [1, 6], "find": 1, "mathemat": 1, "descript": 1, "here": 1, "For": [1, 7], "our": [1, 7], "purpos": 1, "care": 1, "about": 1, "order": 1, "repres": 1, "given": [1, 7], "one": [1, 7], "equat": 1, "tensori": 1, "To": [1, 7], "actual": [1, 6], "comput": 1, "tensor": [1, 4, 5, 8], "represent": 1, "e": [1, 6, 7], "arrai": 1, "number": 1, "form": [1, 7], "requir": [1, 6, 7], "u": [1, 5, 6, 7], "pick": 1, "gener": 1, "sens": 1, "limit": 1, "ourselv": 1, "thing": [1, 8], "an": [1, 7], "infinit": 1, "unfortun": 1, "each": [1, 5, 6], "result": [1, 7], "differ": [1, 6], "The": [1, 6, 7], "point": [1, 6, 7], "being": 1, "programat": 1, "express": 1, "which": [1, 5, 6, 7], "us": [1, 5, 6, 7], "seri": [1, 6], "convei": 1, "inform": 1, "follow": 1, "list": 1, "went": 1, "re": 1, "all": [1, 6, 7], "": 1, "support": 1, "arbitrari": 1, "function": [1, 6, 7], "just": 1, "notabl": 1, "exampl": 1, "ax": 1, "paramet": 1, "have": [1, 6, 7], "mean": [1, 7], "tell": 1, "part": 1, "typic": 1, "done": 1, "refer": 1, "valu": 1, "particular": 1, "come": 1, "varieti": 1, "type": [1, 6, 7], "implicit": 1, "convent": 1, "label": 1, "g": [1, 6], "x": 1, "axi": 1, "alpha": 1, "explicit": 1, "mani": 1, "numer": 1, "primit": 1, "gaussian": 1, "plane": 1, "wave": 1, "mix": 1, "These": [1, 7], "both": [1, 7], "dipol": 1, "moment": 1, "integr": 1, "transform": 1, "from": [1, 5, 6, 7], "anoth": 1, "contract": 1, "properti": 1, "chosen": 1, "random": 1, "rather": 1, "choos": 1, "satisfi": 1, "includ": 1, "diagon": 1, "fock": 1, "matrix": 1, "electron": 1, "densiti": 1, "depend": [1, 7], "spars": [1, 4, 5], "map": [1, 4, 5, 7], "relationship": 1, "dispatch": 1, "construct": 1, "should": 1, "aim": 1, "while": 1, "still": 1, "contain": [1, 7], "suffici": 1, "ensur": 1, "onli": 1, "call": 1, "possess": 1, "correct": 1, "If": [1, 7], "algorithm": [1, 8], "assum": [1, 7], "provid": [1, 7], "want": [1, 7], "reflect": 1, "inherit": 1, "also": 1, "striven": 1, "hierarchi": [1, 8], "wai": [1, 5, 7], "theori": 1, "possibl": 1, "restrict": 1, "unrestrict": 1, "same": [1, 6, 7], "ha": 1, "perform": 1, "consequ": 1, "you": 1, "oper": [1, 6], "multipl": [1, 6], "time": 1, "It": [1, 6], "yet": 1, "been": 1, "implement": [1, 6], "primarili": 1, "lot": 1, "necessari": 1, "That": 1, "said": 1, "terminologi": 1, "alreadi": 1, "exist": [1, 6, 7], "prior": 1, "distinct": 1, "suggest": 1, "among": 1, "physic": [1, 5], "compris": 1, "envis": [1, 6], "heavi": 1, "lift": 1, "fig": 1, "1": [1, 7], "show": 1, "try": 1, "distil": 1, "variou": 1, "down": 1, "know": 1, "love": 1, "becom": 1, "more": [1, 7], "less": [1, 7], "strong": 1, "distinguish": 1, "esoter": 1, "At": [1, 7], "basespac": 1, "meant": 1, "code": [1, 7], "factor": [1, 6], "few": 1, "place": [1, 6], "where": 1, "write": 1, "veri": 1, "n": 1, "cartesianspac": 1, "dimension": 1, "mathbb": [1, 5], "r": 1, "state": 1, "stem": 1, "conceptu": 1, "similar": 1, "spinspac": 1, "span": 1, "most": [1, 7], "common": [1, 6], "scenario": [1, 7], "instanc": [1, 7], "thu": [1, 7], "two": [1, 7], "total": 1, "would": [1, 6], "2": 1, "aospac": 1, "import": 1, "calcul": 1, "becaus": 1, "sort": 1, "other": [1, 7], "worth": 1, "often": 1, "could": 1, "better": 1, "consolid": 1, "aobasisset": 1, "describ": 1, "those": 1, "primitivegaussianspac": 1, "hold": [1, 6], "center": 1, "angular": 1, "momenta": 1, "expon": 1, "cgtospac": 1, "transformedspac": 1, "t": [1, 7], "store": [1, 5, 6], "coeffici": 1, "go": [1, 7], "sphericalgtospac": 1, "spheric": 1, "structur": 1, "packag": 1, "concept": [1, 6], "inter": 1, "interoper": 1, "facilit": 1, "nonetheless": 1, "case": 1, "benefit": 1, "deviat": 1, "standard": 1, "relativist": 1, "un": 1, "mai": [1, 7], "revisit": 1, "interest": 1, "than": [1, 7], "ones": 1, "linear": 1, "derivedspac": 1, "obtain": 1, "rotat": 1, "preserv": 1, "length": 1, "angl": 1, "don": 1, "strictli": 1, "enforc": 1, "serv": 1, "introduc": 1, "simpli": 1, "There": [1, 7], "sever": 1, "take": [1, 7], "productspac": 1, "so": 1, "account": 1, "fact": 1, "thei": 1, "simplifi": 1, "involv": [1, 6], "compar": 1, "object": [1, 6, 7], "addit": [1, 6], "naturalspac": 1, "b": 1, "directli": 1, "allow": 1, "user": [1, 7], "specifi": 1, "below": 1, "localizedspac": 1, "denot": [1, 5], "spatial": 1, "accord": 1, "some": 1, "metric": 1, "present": [1, 7], "discern": 1, "templat": [1, 6], "ani": 1, "exploit": 1, "sparsiti": [1, 5, 6], "member": [1, 5], "non": 1, "zero": 1, "element": [1, 6, 7], "first": [1, 7], "domain": [1, 5, 7, 8], "second": [1, 7], "independentspac": 1, "rel": 1, "quintessenti": 1, "program": 1, "stand": 1, "think": 1, "name": 1, "resid": 1, "asospac": 1, "combin": 1, "afaiu": 1, "start": [1, 7], "formul": 1, "action": 1, "typedef": 1, "anyth": 1, "els": 1, "beta": 1, "2n": 1, "spinorspac": 1, "whose": 1, "get": [1, 7], "turn": 1, "msospac": 1, "sinc": [1, 7], "out": [1, 6, 7], "block": 1, "lead": 1, "assumpt": [1, 8], "captur": 1, "mospac": 1, "consist": 1, "correl": 1, "method": 1, "cmospac": 1, "energi": 1, "nospac": 1, "occup": 1, "reli": 1, "singl": [1, 6], "rais": 1, "problem": 1, "NOT": 1, "therefor": 1, "pass": [1, 7], "expect": 1, "least": 1, "solut": 1, "convers": [1, 7], "ow": 1, "complex": 1, "opt": 1, "latter": 1, "over": [1, 7], "year": 1, "theorist": 1, "influenc": 1, "approxim": 1, "consum": 1, "make": 1, "wa": 1, "simplest": 1, "manner": [1, 7], "adher": 1, "respons": 1, "summar": 1, "doe": 1, "new": 1, "anytim": 1, "definit": 1, "found": 1, "throughout": 1, "encourag": 1, "lowest": 1, "level": 1, "closest": 1, "ultim": 1, "establish": 1, "between": 1, "sai": [1, 7], "alon": 1, "modul": 1, "c": [1, 4], "system": 1, "how": [1, 3, 6], "respect": 1, "develop": [1, 4], "whatev": 1, "simul": 1, "whole": 1, "host": 1, "flesh": 1, "accommod": 1, "when": 1, "concern": [3, 6], "help": 3, "reader": 3, "understand": 3, "design": [3, 4, 8], "contribut": 3, "repositori": 3, "nwchemex": [3, 4], "organ": [3, 4], "sublibrari": 4, "sparsemap": [4, 7, 8], "background": [4, 8], "sparsifi": [4, 8], "api": [4, 6], "speak": 5, "notat": 5, "l": 5, "v": [5, 6], "rightarrow": 5, "subspac": 5, "its": 5, "let": 5, "_": 5, "A": 5, "u_v": 5, "tile": [6, 7], "storag": 6, "rang": 6, "etc": 6, "encapsul": 6, "basic": 6, "union": 6, "intersect": 6, "domaintrait": 6, "domainpimpl": 6, "domainbas": 6, "charg": 6, "model": 6, "indic": [6, 7], "By": 6, "chang": 6, "data": [6, 7], "pimpl": 6, "later": [6, 7], "memori": 6, "semant": 6, "implicitli": 6, "separ": 6, "refactor": 6, "without": 6, "itself": 6, "either": 6, "special": [6, 7], "index": [6, 7], "normal": 6, "redefin": 6, "instead": 6, "mirror": 6, "reason": [6, 7], "Of": 6, "note": [6, 8], "four": 6, "elementindex": [6, 7], "tileindex": 6, "detail": 7, "creat": 7, "distarrai": 7, "copi": 7, "best": 7, "abl": 7, "full": 7, "worst": 7, "piec": 7, "my": 7, "knowledg": 7, "alia": 7, "must": 7, "occur": 7, "main": 7, "kernel": 7, "afford": 7, "flexibl": 7, "ad": 7, "sm": 7, "goal": 7, "tot": 7, "outer": 7, "rank": 7, "ind_rank": 7, "inner": 7, "dep_rank": 7, "class": [7, 8], "now": 7, "equal": 7, "former": 7, "valid": 7, "concret": 7, "0": 7, "retriev": 7, "oppos": 7, "offset": 7, "permut": 7, "convert": 7, "inject": 7, "independ": 7, "mode": 7, "word": 7, "reduc": 7, "slice": 7, "insert": 7, "unifi": 7, "realiz": 7, "long": 7, "up": 7, "work": 7, "cover": 7, "gut": 7, "from_sparse_tensor": 7, "lambda": 7, "ta": 7, "make_arrai": 7, "fill": 7, "std": 7, "ind2mod": 7, "th": 7, "loop": 7, "oeidx": 7, "oedix": 7, "empti": 7, "move": 7, "otherwis": 7, "alloc": 7, "buffer": 7, "injected_d": 7, "tdomain": 7, "itidx": 7, "ieidx": 7, "add": 7, "return": 7, "kei": 8, "make_tot_tile_": 8}, "objects": {}, "objtypes": {}, "objnames": {}, "titleterms": {"design": [0, 1, 2, 6], "document": [0, 3], "content": [0, 3, 4, 8], "vector": 1, "space": 1, "what": 1, "ar": 1, "why": 1, "do": 1, "we": 1, "need": 1, "consider": [1, 6], "fundament": 1, "class": [1, 6], "base": 1, "cartesian": 1, "spin": 1, "atom": 1, "orbit": 1, "ao": 1, "deriv": 1, "product": 1, "natur": 1, "local": 1, "independ": 1, "aso": 1, "spinor": 1, "molecular": 1, "mso": 1, "mo": 1, "canon": 1, "cmo": 1, "NOs": 1, "note": [1, 7], "side": 1, "cast": 1, "summari": 1, "futur": 1, "wavefunct": 2, "develop": 3, "chemist": 4, "sparsemap": [5, 6], "background": 5, "spars": [6, 8], "map": [6, 8], "librari": 6, "kei": 6, "domain": 6, "hierarchi": 6, "sparsifi": 7, "tensor": 7, "thing": 7, "assumpt": 7, "make_tot_tile_": 7, "algorithm": 7, "sublibrari": 8}, "envversion": {"sphinx.domains.c": 2, "sphinx.domains.changeset": 1, "sphinx.domains.citation": 1, "sphinx.domains.cpp": 8, "sphinx.domains.index": 1, "sphinx.domains.javascript": 2, "sphinx.domains.math": 2, "sphinx.domains.python": 3, "sphinx.domains.rst": 2, "sphinx.domains.std": 2, "sphinxcontrib.bibtex": 9, "sphinx": 57}, "alltitles": {"Design Documentation": [[0, "design-documentation"]], "Contents:": [[0, null], [3, null], [4, null]], "Vector Space Design": [[1, "vector-space-design"], [1, "id1"]], "What are vector spaces?": [[1, "what-are-vector-spaces"]], "Why do we need vector spaces?": [[1, "why-do-we-need-vector-spaces"]], "Vector Space Considerations": [[1, "vector-space-considerations"]], "Fundamental Classes": [[1, "fundamental-classes"]], "Base Class": [[1, "base-class"]], "Cartesian Space": [[1, "cartesian-space"]], "Spin Space": [[1, "spin-space"]], "Atomic Orbitals (AOs)": [[1, "atomic-orbitals-aos"]], "Derived Space": [[1, "derived-space"]], "Product Space": [[1, "product-space"]], "Natural Space": [[1, "natural-space"]], "Localized Space": [[1, "localized-space"]], "Independent Spaces": [[1, "independent-spaces"]], "Orbital Space Classes": [[1, "orbital-space-classes"]], "Atomic Spin Orbital (ASO) Space": [[1, "atomic-spin-orbital-aso-space"]], "Spinor Space": [[1, "spinor-space"]], "Molecular Spin Orbitals (MSOs)": [[1, "molecular-spin-orbitals-msos"]], "Molecular Orbitals (MOs)": [[1, "molecular-orbitals-mos"]], "Canonical Molecular Orbitals (CMOs)": [[1, "canonical-molecular-orbitals-cmos"]], "Natural Orbitals (NOs)": [[1, "natural-orbitals-nos"]], "Notes on \u201cSide Casts\u201d": [[1, "notes-on-side-casts"]], "Vector Space Summary": [[1, "vector-space-summary"]], "Future Considerations": [[1, "future-considerations"]], "Wavefunction Design": [[2, "wavefunction-design"]], "Developer Documentation": [[3, "developer-documentation"]], "Chemist": [[4, "chemist"]], "SparseMap Background": [[5, "sparsemap-background"]], "Sparse Map Library Design": [[6, "sparse-map-library-design"]], "Key Considerations": [[6, "key-considerations"]], "Domain Class Hierarchy": [[6, "domain-class-hierarchy"]], "SparseMap Class Hierarchy": [[6, "sparsemap-class-hierarchy"]], "Sparsifying a Tensor": [[7, "sparsifying-a-tensor"]], "Things to note": [[7, "things-to-note"]], "Assumptions": [[7, "assumptions"]], "make_tot_tile_ Algorithm": [[7, "make-tot-tile-algorithm"]], "Sparse Maps Sublibrary": [[8, "sparse-maps-sublibrary"]], "Contents": [[8, null]]}, "indexentries": {}})