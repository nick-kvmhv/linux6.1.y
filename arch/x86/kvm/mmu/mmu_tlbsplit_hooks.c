/*
 * TLB Split MMU Hooks
 * Included directly at the end of mmu.c to access private shadow paging iterators.
 */

u64* split_tlb_findspte(struct kvm_vcpu *vcpu, gfn_t gfn, int callback(u64* sptep, int level, int last, int large))
{
	struct kvm_shadow_walk_iterator iterator;

	for_each_shadow_entry(vcpu, gfn << PAGE_SHIFT, iterator) {
		u64 spte = iterator.sptep ? split_tlb_safe_deref(iterator.sptep) : 0;
		if (spte == 0)
			break;
		if (spte != 0) {
			int last = is_last_spte(spte, iterator.level);
			int large = is_large_pte(spte);
			if (callback(iterator.sptep, iterator.level, last, large))
				return iterator.sptep;
		}
	}
	return NULL;
}
EXPORT_SYMBOL_GPL(split_tlb_findspte);