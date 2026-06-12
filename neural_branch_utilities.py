from gurobi_utilities import *
from model import *

def gnn_topk_candidates(
    model,
    integer_var,
    instance,
    fractional_vars,
    policy,
    device,
    top_k=10,
):


    if len(fractional_vars) == 0:
        return np.asarray([], dtype=np.int64)

    state = extract_gurobi_state(
        model=model,
        integer_var=integer_var,
        instance=instance,
    )

    constraint_features_dict, edge_features_dict, variable_features_dict = state

    constraint_features = torch.from_numpy(
        constraint_features_dict["values"]
    ).float().to(device)

    edge_index = torch.from_numpy(
        edge_features_dict["indices"]
    ).long().to(device)

    edge_attr = torch.from_numpy(
        edge_features_dict["values"]
    ).float().to(device)

    variable_features = torch.from_numpy(
        variable_features_dict["values"]
    ).float().to(device)

    fractional_vars_t = torch.from_numpy(
        np.asarray(fractional_vars, dtype=np.int64)
    ).long().to(device)

    with torch.no_grad():
        out = policy(
            constraint_features,
            edge_index,
            edge_attr,
            variable_features,
        )

        # If using the newer model that can return (logits, embeddings)
        if isinstance(out, tuple):
            logits = out[0]
        else:
            logits = out

        candidate_logits = logits[fractional_vars_t]

        k = min(top_k, len(fractional_vars))
        top_local = torch.topk(candidate_logits, k=k, dim=0).indices

        top_candidates = fractional_vars_t[top_local].detach().cpu().numpy()

    return top_candidates.astype(np.int64)


def select_gnn_filtered_strong_branching_var(
    model,
    integer_var,
    instance,
    x_candidate,
    parent_obj,
    fractional_vars,
    policy,
    device,
    top_k=10,
):
    """
    Hybrid brancher:
        1. GNN ranks all fractional candidates.
        2. Keep top_k candidates.
        3. Run strong branching only on those top_k.
        4. Return best strong-branching candidate.
    """

    top_candidates = gnn_topk_candidates(
        model=model,
        integer_var=integer_var,
        instance=instance,
        fractional_vars=fractional_vars,
        policy=policy,
        device=device,
        top_k=top_k,
    )

    if len(top_candidates) == 0:
        return None

    scores = strong_branching_scores_gurobi(
        model=model,
        candidates=top_candidates,
        x_candidate=np.asarray(x_candidate, dtype=np.float64),
        parent_obj=parent_obj,
        child_time_limit=2.0,
    )

    selected_var_idx = int(top_candidates[np.argmax(scores)])

    return selected_var_idx


def load_gnn_policy(
    model_path="models/cvrp_gnn/gnn_policy.pt",
    device=None,
):
    if device is None:
        device = "cuda" if torch.cuda.is_available() else "cpu"

    policy = GNNPolicy(
        var_nfeats=19,
        cons_nfeats=5,
        edge_nfeats=1,
        emb_size=32,
        n_rounds=2,
        dropout=0.05,
    ).to(device)

    checkpoint = torch.load(
        model_path,
        map_location=device,
    )

    if isinstance(checkpoint, dict) and "policy" in checkpoint:
        state_dict = checkpoint["policy"]
    else:
        state_dict = checkpoint

    policy.load_state_dict(state_dict)
    policy.eval()

    return policy, device