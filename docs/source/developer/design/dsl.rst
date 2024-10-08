#############
DSL Component
#############

These are notes on designing the DSL component.

- We'll need DSLs for combining wavefunctions, operators, and potentially 
  BraKets and chemical systems.
- This component is an attempt to create a reusable DSL framework. It stops
  shy of evaluating the expression that is created.
- We'll have everything use CRTP to inherit from dsl::Term, that way things
  can just take ``Term<T>`` objects and then parse the graph described by
  the object.
- Note that to get const-ness correct the ops need to be templated (e.g., the
  "reference" type of an op needs to mutable if it wraps a mutable object and
  read-only if it wraps a read-only object). This requires TMP to work out the
  correct type.

- To use the DSL promote an object in your

  .. code-block:: C++

     class OperatorImpl<DerivedType, ...>{
     public:
         template<typename DerivedType2, ...>
         auto operator+(const OperatorImpl<DerivedType2>& rhs) const {
             return dsl::Add<DerivedType, DerivedType2>(*this, rhs);
         }
